#include "daScript/daScript.h"
#include "daScript/ast/ast_handle.h"
#include "daScript/ast/ast_interop.h"
#include "daScript/ast/ast_typefactory_bind.h"
#include "daScript/misc/sysos.h"
#include "daScript/simulate/aot.h"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>

struct Double3 {
    double x;
    double y;
    double z;
};

MAKE_TYPE_FACTORY(Double3, Double3)

namespace das {
vector<void *> force_aot_stub();
}

Double3 make_double3(double x, double y, double z) {
    return Double3{x, y, z};
}

Double3 add_double3(const Double3& lhs, const Double3& rhs) {
    return Double3{lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z};
}

Double3 scale_double3(const Double3& v, double s) {
    return Double3{v.x * s, v.y * s, v.z * s};
}

Double3 cpp_transform(const Double3& v) {
    return Double3{v.z + 10.0, v.x + 20.0, v.y + 30.0};
}

void print_double3_from_cpp(const Double3& v) {
    std::cout << "[cpp] Double3(x=" << v.x << ", y=" << v.y << ", z=" << v.z << ")\n";
}

namespace das {

struct Double3Annotation final : ManagedStructureAnnotation<Double3, false> {
    explicit Double3Annotation(ModuleLibrary& lib) : ManagedStructureAnnotation("Double3", lib) {
        addField<DAS_BIND_MANAGED_FIELD(x)>("x", "x");
        addField<DAS_BIND_MANAGED_FIELD(y)>("y", "y");
        addField<DAS_BIND_MANAGED_FIELD(z)>("z", "z");
    }
};

class Module_Double3Demo final : public Module {
public:
    Module_Double3Demo() : Module("double3_demo") {
        ModuleLibrary lib(this);
        lib.addBuiltInModule();

        addAnnotation(make_smart<Double3Annotation>(lib));

        addExtern<DAS_BIND_FUN(make_double3), SimNode_ExtFuncCallAndCopyOrMove>(
            *this,
            lib,
            "make_double3",
            SideEffects::none,
            "make_double3"
        )
            ->args({"x", "y", "z"});

        addExtern<DAS_BIND_FUN(add_double3), SimNode_ExtFuncCallAndCopyOrMove>(
            *this,
            lib,
            "add_double3",
            SideEffects::none,
            "add_double3"
        )
            ->args({"lhs", "rhs"});

        addExtern<DAS_BIND_FUN(scale_double3), SimNode_ExtFuncCallAndCopyOrMove>(
            *this,
            lib,
            "scale_double3",
            SideEffects::none,
            "scale_double3"
        )
            ->args({"v", "s"});

        addExtern<DAS_BIND_FUN(cpp_transform), SimNode_ExtFuncCallAndCopyOrMove>(
            *this,
            lib,
            "cpp_transform",
            SideEffects::none,
            "cpp_transform"
        )
            ->args({"v"});

        addExtern<DAS_BIND_FUN(print_double3_from_cpp)>(
            *this,
            lib,
            "print_double3_from_cpp",
            SideEffects::modifyExternal,
            "print_double3_from_cpp"
        )
            ->args({"v"});

        verifyAotReady();
    }
};

}  // namespace das

REGISTER_MODULE_IN_NAMESPACE(Module_Double3Demo, das)

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

RunStats run_once(const std::string& scriptPath, int iterations, bool useAot) {
    TextPrinter tout;
    ModuleGroup moduleGroup;
    auto fileAccess = make_smart<FsFileAccess>();

    CodeOfPolicies policies;
    policies.aot = useAot;
    policies.fail_on_no_aot = false;

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
    std::cout << "[host] iterations=" << iterations << ", rounds=" << rounds << "\n";

    try {
        long long interpTotal = 0;
        long long aotTotal = 0;

        std::cout << "\n=== Interpreter runs ===\n";
        for (int i = 0; i < rounds; ++i) {
            const auto stats = run_once(scriptPath, iterations, false);
            interpTotal += stats.elapsedMs;
            std::cout << "[interpreter] round=" << i + 1 << " elapsed_ms=" << stats.elapsedMs << "\n";
            print_double3_from_cpp(stats.value);
        }

        std::cout << "\n=== AOT runs ===\n";
        for (int i = 0; i < rounds; ++i) {
            const auto stats = run_once(scriptPath, iterations, true);
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
