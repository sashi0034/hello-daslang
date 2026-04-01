#include "daScript/daScript.h"
#include "daScript/simulate/aot.h"
#include "daScript/misc/sysos.h"

#include <cstdint>
#include <iostream>
#include <string>

using namespace das;

namespace {
    std::string normalize_path(std::string path) {
        for (char & ch : path) {
            if (ch == '\\') {
                ch = '/';
            }
        }
        return path;
    }

    void print_program_errors(TextPrinter & tout, const ProgramPtr & program) {
        for (const auto & err : program->errors) {
            tout << reportError(err.at, err.what, err.extra, err.fixme, err.cerr);
        }
    }
}

int main(int, char * []) {
    const auto projectRoot = normalize_path(HELLO_DASLANG_PROJECT_ROOT);
    const auto dasRoot = normalize_path(HELLO_DASLANG_DAS_ROOT);
    const auto scriptPath = projectRoot + "/scripts/hello.das";

    NEED_ALL_DEFAULT_MODULES;
    Module::Initialize();
    setDasRoot(dasRoot);

    auto shutdown = das_finally([&]() {
        Module::Shutdown();
    });

    TextPrinter tout;
    ModuleGroup moduleGroup;
    auto fileAccess = make_smart<FsFileAccess>();

    auto program = compileDaScript(scriptPath, fileAccess, tout, moduleGroup);
    if (program->failed()) {
        std::cerr << "daScript compilation failed\n";
        print_program_errors(tout, program);
        return 1;
    }

    Context context(program->getContextStackSize());
    if (!program->simulate(context, tout)) {
        std::cerr << "daScript simulation failed\n";
        print_program_errors(tout, program);
        return 1;
    }

    auto helloFn = context.findFunction("hello_from_cpp");
    if (!helloFn) {
        std::cerr << "Function 'hello_from_cpp' was not found\n";
        return 1;
    }
    Func helloFunc(helloFn);

    if (!verifyCall<void, int32_t, char *>(helloFn->debugInfo, moduleGroup)) {
        std::cerr << "Function signature mismatch for hello_from_cpp\n";
        return 1;
    }

    das_invoke_function<void>::invoke(
        &context,
        nullptr,
        helloFunc,
        int32_t(21),
        const_cast<char *>("from C++ host")
    );

    if (auto ex = context.getException()) {
        std::cerr << "daScript exception: " << ex << "\n";
        return 1;
    }

    auto makeMessageFn = context.findFunction("make_message");
    if (!makeMessageFn) {
        std::cerr << "Function 'make_message' was not found\n";
        return 1;
    }
    Func makeMessageFunc(makeMessageFn);

    if (!verifyCall<char *, char *>(makeMessageFn->debugInfo, moduleGroup)) {
        std::cerr << "Function signature mismatch for make_message\n";
        return 1;
    }

    const char * message = das_invoke_function<const char *>::invoke(
        &context,
        nullptr,
        makeMessageFunc,
        const_cast<char *>("second call")
    );

    if (auto ex = context.getException()) {
        std::cerr << "daScript exception: " << ex << "\n";
        return 1;
    }

    std::cout << "Returned from daScript: " << message << "\n";
    return 0;
}
