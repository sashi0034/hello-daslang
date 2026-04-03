#include "daScript/daScript.h"
#include "daScript/ast/ast_typefactory_bind.h"
#include "daScript/misc/sysos.h"
#include "daScript/simulate/aot.h"
#include "double3_bindings.h"
#include "double3_module.h"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>

namespace das {
vector<void *> force_aot_stub();
}

MAKE_TYPE_FACTORY(Double3, Double3)

namespace {
using namespace das;


struct RunStats {
    Double3 value{};
    long long elapsedMs = 0;
};

std::string normalize_path(std::string path) {
    for (char& ch : path) {
        if (ch == '\\') {
            ch = '/';
        }
    }
    return path;
}

void print_program_errors(TextPrinter& tout, const ProgramPtr& program) {
    for (const auto& err : program->errors) {
        tout << reportError(err.at, err.what, err.extra, err.fixme, err.cerr);
    }
}

int parse_int_arg(int argc, char* argv[], int index, int fallback) {
    if (argc <= index) {
        return fallback;
    }
    try {
        return std::max(1, std::stoi(argv[index]));
    } catch (...) {
        return fallback;
    }
}

bool parse_bool_arg(int argc, char* argv[], int index, bool fallback) {
    if (argc <= index) {
        return fallback;
    }
    try {
        return std::stoi(argv[index]) != 0;
    } catch (...) {
        return fallback;
    }
}


RunStats run_once(const std::string& scriptPath, int iterations, bool useAot, bool strictAot) {
    TextPrinter tout;
    ModuleGroup moduleGroup;
    auto fileAccess = make_smart<FsFileAccess>();

    CodeOfPolicies policies;
    policies.aot = useAot;
    policies.fail_on_no_aot = useAot && strictAot;

    auto program = compileDaScript(scriptPath, fileAccess, tout, moduleGroup, policies);
    if (program->failed()) {
        print_program_errors(tout, program);
        throw std::runtime_error(useAot ? "AOT compilation failed" : "Interpreter compilation failed");
    }

    auto context = std::make_unique<Context>(program->getContextStackSize());
    if (!program->simulate(*context, tout)) {
        print_program_errors(tout, program);
        throw std::runtime_error(useAot ? "AOT simulate failed" : "Interpreter simulate failed");
    }

    auto runEntry = context->findFunction("run_entry");
    if (!runEntry) {
        throw std::runtime_error("Function 'run_entry' not found");
    }

    if (!verifyCall<Double3, int32_t>(runEntry->debugInfo, moduleGroup)) {
        throw std::runtime_error("Signature mismatch for run_entry(work_iters:int):Double3");
    }

    const auto t0 = std::chrono::steady_clock::now();
    const auto result = das_invoke_function<Double3>::invoke_cmres(context.get(), nullptr, Func(runEntry), iterations);
    const auto t1 = std::chrono::steady_clock::now();

    if (const auto ex = context->getException()) {
        throw std::runtime_error(std::string("daScript exception: ") + ex);
    }

    return RunStats{
        result,
        std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count(),
    };
}

}  // namespace

int main(int argc, char* argv[]) {
    using namespace das;

    const auto iterations = parse_int_arg(argc, argv, 1, 300000);
    const auto rounds = parse_int_arg(argc, argv, 2, 3);
    const bool strictAot = parse_bool_arg(argc, argv, 3, false);

    const auto projectRoot = normalize_path(HELLO_DASLANG_PROJECT_ROOT);
    const auto dasRoot = normalize_path(HELLO_DASLANG_DAS_ROOT);
    const auto scriptPath = projectRoot + "/scripts/double3_demo.das";

    NEED_ALL_DEFAULT_MODULES;
    NEED_MODULE(Module_Double3Demo);

    Module::Initialize();
    setDasRoot(dasRoot);
    force_aot_stub();

    auto shutdown = das_finally([]() { Module::Shutdown(); });

    std::cout << "[host] script=" << scriptPath << "\n";
    std::cout << "[host] iterations=" << iterations << ", rounds=" << rounds
              << ", strict_aot=" << (strictAot ? "true" : "false") << "\n";

    try {
        long long interpTotal = 0;
        long long aotTotal = 0;

        std::cout << "\n=== Interpreter runs ===\n";
        for (int i = 0; i < rounds; ++i) {
            const auto stats = run_once(scriptPath, iterations, false, strictAot);
            interpTotal += stats.elapsedMs;
            std::cout << "[interpreter] round=" << i + 1 << " elapsed_ms=" << stats.elapsedMs << "\n";
            print_double3_from_cpp(stats.value);
        }

        std::cout << "\n=== AOT runs ===\n";
        for (int i = 0; i < rounds; ++i) {
            const auto stats = run_once(scriptPath, iterations, true, strictAot);
            aotTotal += stats.elapsedMs;
            std::cout << "[aot] round=" << i + 1 << " elapsed_ms=" << stats.elapsedMs << "\n";
            print_double3_from_cpp(stats.value);
        }

        const auto interpAvg = static_cast<double>(interpTotal) / static_cast<double>(rounds);
        const auto aotAvg = static_cast<double>(aotTotal) / static_cast<double>(rounds);

        std::cout << "\n=== Summary ===\n";
        std::cout << "[summary] interpreter_avg_ms=" << interpAvg << "\n";
        std::cout << "[summary] aot_avg_ms=" << aotAvg << "\n";
        if (aotAvg > 0.0) {
            std::cout << "[summary] speedup(interpreter/aot)=" << (interpAvg / aotAvg) << "x\n";
        }
    } catch (const std::exception& ex) {
        std::cerr << "[host] error: " << ex.what() << "\n";
        return 1;
    }

    return 0;
}
