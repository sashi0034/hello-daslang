add_executable(double3_aot_sandbox
    src/main.cpp
)

target_compile_definitions(double3_aot_sandbox PRIVATE
    DAS_MOD_EXPORTS
    HELLO_DASLANG_REPO_ROOT="${HELLO_DASLANG_REPO_ROOT}"
    HELLO_DASLANG_PROJECT_ROOT="${CMAKE_CURRENT_SOURCE_DIR}"
    HELLO_DASLANG_DAS_ROOT="${HELLO_DASLANG_DAS_ROOT}"
)

target_include_directories(double3_aot_sandbox PRIVATE
    "${HELLO_DASLANG_DAS_ROOT}/include"
    "${CMAKE_BINARY_DIR}/vendor/daScript/include"
)

target_link_libraries(double3_aot_sandbox PRIVATE
    libDaScriptAot
    libDaScript
    Threads::Threads
)

target_compile_features(double3_aot_sandbox PRIVATE cxx_std_17)

if(MSVC)
    target_compile_options(double3_aot_sandbox PRIVATE /wd4005)
endif()

set_target_properties(double3_aot_sandbox PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)
