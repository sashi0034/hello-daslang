#include "daScript/daScript.h"
#include "daScript/simulate/aot.h"
#include "daScript/misc/sysos.h"

#include <chrono>
#include <filesystem>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <thread>

using namespace das;

namespace
{
    namespace fs = std::filesystem;

    struct ScriptRuntime
    {
        ProgramPtr program;
        std::unique_ptr<Context> context;
        Func tickFunction;
        Func saveTickCountFunction;
        Func saveAccumulatorFunction;
        Func restoreStateFunction;
    };

    struct ScriptSnapshot
    {
        int32_t localTickCount = 0;
        float accumulator = 0.0f;
    };

    std::string normalize_path(std::string path)
    {
        for (char& ch : path)
        {
            if (ch == '\\')
            {
                ch = '/';
            }
        }
        return path;
    }

    void print_program_errors(TextPrinter& tout, const ProgramPtr& program)
    {
        for (const auto& err : program->errors)
        {
            tout << reportError(err.at, err.what, err.extra, err.fixme, err.cerr);
        }
    }

    bool load_script(
        const std::string& scriptPath,
        ScriptRuntime& runtime,
        std::string& errorMessage
    )
    {
        TextPrinter tout;
        ModuleGroup moduleGroup;
        auto fileAccess = make_smart<FsFileAccess>();
        auto program = compileDaScript(scriptPath, fileAccess, tout, moduleGroup);

        if (program->failed())
        {
            print_program_errors(tout, program);
            errorMessage = "daScript compilation failed";
            return false;
        }

        auto context = std::make_unique<Context>(program->getContextStackSize());
        if (!program->simulate(*context, tout))
        {
            print_program_errors(tout, program);
            errorMessage = "daScript simulation failed";
            return false;
        }

        auto tickFn = context->findFunction("tick");
        if (!tickFn)
        {
            errorMessage = "Function 'tick' was not found";
            return false;
        }

        if (!verifyCall<void, float, int32_t, float, int32_t>(tickFn->debugInfo, moduleGroup))
        {
            errorMessage =
                "Function signature mismatch for tick(dt: float; frame: int; uptime: float; reload_count: int)";
            return false;
        }

        runtime.program = program;
        runtime.context = std::move(context);
        runtime.tickFunction = Func(tickFn);
        runtime.saveTickCountFunction = Func(runtime.context->findFunction("save_state_tick_count"));
        runtime.saveAccumulatorFunction = Func(runtime.context->findFunction("save_state_accumulator"));
        runtime.restoreStateFunction = Func(runtime.context->findFunction("restore_state"));
        errorMessage.clear();
        return true;
    }

    std::optional<ScriptSnapshot> capture_snapshot(ScriptRuntime& runtime)
    {
        if (!runtime.saveTickCountFunction || !runtime.saveAccumulatorFunction)
        {
            return std::nullopt;
        }

        ScriptSnapshot snapshot;
        snapshot.localTickCount = das_invoke_function<int32_t>::invoke(
            runtime.context.get(),
            nullptr,
            runtime.saveTickCountFunction
        );
        snapshot.accumulator = das_invoke_function<float>::invoke(
            runtime.context.get(),
            nullptr,
            runtime.saveAccumulatorFunction
        );
        return snapshot;
    }

    bool restore_snapshot(ScriptRuntime& runtime, const ScriptSnapshot& snapshot)
    {
        if (!runtime.restoreStateFunction)
        {
            return false;
        }

        das_invoke_function<void>::invoke(
            runtime.context.get(),
            nullptr,
            runtime.restoreStateFunction,
            snapshot.localTickCount,
            snapshot.accumulator
        );
        return runtime.context->getException() == nullptr;
    }

    fs::file_time_type safe_last_write_time(const fs::path& path)
    {
        std::error_code ec;
        const auto time = fs::last_write_time(path, ec);
        if (ec)
        {
            return fs::file_time_type::min();
        }
        return time;
    }

    std::optional<int32_t> parse_max_frames(int argc, char* argv[])
    {
        if (argc < 2)
        {
            return std::nullopt;
        }

        try
        {
            return static_cast<int32_t>(std::stoi(argv[1]));
        }
        catch (...)
        {
            return std::nullopt;
        }
    }
}

int main(int argc, char* argv[])
{
    const auto projectRoot = normalize_path(HELLO_DASLANG_PROJECT_ROOT);
    const auto dasRoot = normalize_path(HELLO_DASLANG_DAS_ROOT);
    const auto scriptPath = projectRoot + "/scripts/hotreload_tick.das";
    const auto scriptFile = fs::path(scriptPath);

    NEED_ALL_DEFAULT_MODULES;
    Module::Initialize();
    setDasRoot(dasRoot);

    auto shutdown = das_finally([&]()
    {
        Module::Shutdown();
    });

    ScriptRuntime runtime;
    std::string errorMessage;
    if (!load_script(scriptPath, runtime, errorMessage))
    {
        std::cerr << errorMessage << "\n";
        return 1;
    }

    auto observedWriteTime = safe_last_write_time(scriptFile);
    int32_t frame = 0;
    int32_t reloadCount = 0;
    const auto maxFrames = parse_max_frames(argc, argv);
    constexpr auto frameDuration = std::chrono::milliseconds(500);
    const auto startTime = std::chrono::steady_clock::now();

    std::cout << "Watching " << scriptPath << "\n";
    std::cout << "Edit the script while this process is running. Press Ctrl+C to stop.\n";
    if (maxFrames)
    {
        std::cout << "The host will exit after " << *maxFrames << " frames.\n";
    }

    for (;;)
    {
        const auto latestWriteTime = safe_last_write_time(scriptFile);
        if (latestWriteTime != fs::file_time_type::min() && latestWriteTime != observedWriteTime)
        {
            ScriptRuntime reloadedRuntime;
            std::string reloadError;
            auto snapshot = capture_snapshot(runtime);
            std::cout << "[host] change detected, recompiling script...\n";
            if (load_script(scriptPath, reloadedRuntime, reloadError))
            {
                if (snapshot)
                {
                    if (restore_snapshot(reloadedRuntime, *snapshot))
                    {
                        std::cout << "[host] restored script state (local_tick_count="
                                  << snapshot->localTickCount
                                  << ", accumulator="
                                  << snapshot->accumulator
                                  << ")\n";
                    }
                    else
                    {
                        const auto restoreError = reloadedRuntime.context->getException();
                        std::cerr << "[host] state restore failed";
                        if (restoreError)
                        {
                            std::cerr << ": " << restoreError;
                        }
                        std::cerr << "\n";
                        reloadedRuntime.context->restart();
                    }
                }
                runtime = std::move(reloadedRuntime);
                observedWriteTime = latestWriteTime;
                ++reloadCount;
                std::cout << "[host] reload succeeded (reload_count=" << reloadCount << ")\n";
            }
            else
            {
                std::cerr << "[host] reload failed: " << reloadError << "\n";
                std::cerr << "[host] keeping previous script instance alive\n";
            }
        }

        const auto now = std::chrono::steady_clock::now();
        const auto uptime = std::chrono::duration_cast<std::chrono::duration<float>>(now - startTime).count();

        das_invoke_function<void>::invoke(
            runtime.context.get(),
            nullptr,
            runtime.tickFunction,
            std::chrono::duration_cast<std::chrono::duration<float>>(frameDuration).count(),
            frame,
            uptime,
            reloadCount
        );

        if (const auto ex = runtime.context->getException())
        {
            std::cerr << "[host] daScript exception: " << ex << "\n";
            return 1;
        }

        ++frame;
        if (maxFrames && frame >= *maxFrames)
        {
            std::cout << "[host] reached max frame count, exiting\n";
            break;
        }
        std::this_thread::sleep_for(frameDuration);
    }

    return 0;
}
