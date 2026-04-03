set(DOUBLE3CPP_AOT_GENERATED_SRC)
set(DOUBLE3CPP_AOT_OUTPUT
    "${CMAKE_CURRENT_BINARY_DIR}/double3cpp_aot_sandbox_double3cpp_demo.das.cpp"
)
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
set_source_files_properties("${DOUBLE3CPP_AOT_OUTPUT}" PROPERTIES GENERATED TRUE)

add_executable(double3cpp_aot_tool
    ${DAS_DASCRIPT_MAIN_SRC}
    src/double3_aot_tool.cpp
    src/double3_module.cpp
)

target_compile_definitions(double3cpp_aot_tool PRIVATE
    DAS_MOD_EXPORTS
)

target_include_directories(double3cpp_aot_tool PRIVATE
    "${HELLO_DASLANG_DAS_ROOT}/include"
    "${CMAKE_BINARY_DIR}/vendor/daScript/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(double3cpp_aot_tool PRIVATE
    libDaScriptDyn
    Threads::Threads
)

target_compile_features(double3cpp_aot_tool PRIVATE cxx_std_17)

add_custom_command(TARGET double3cpp_aot_tool POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:libDaScriptDyn>
        $<TARGET_FILE_DIR:double3cpp_aot_tool>
    COMMENT "Copying libDaScriptDyn.dll next to double3cpp_aot_tool"
)

add_custom_command(
    OUTPUT "${DOUBLE3CPP_AOT_OUTPUT}"
    DEPENDS
        double3cpp_aot_tool
        "${CMAKE_CURRENT_SOURCE_DIR}/scripts/double3cpp_demo.das"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/double3_bindings.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/double3_module.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/double3_module.h"
    COMMAND
        double3cpp_aot_tool
        -aot
        "${CMAKE_CURRENT_SOURCE_DIR}/scripts/double3cpp_demo.das"
        "${DOUBLE3CPP_AOT_OUTPUT}"
        -dasroot
        "${HELLO_DASLANG_DAS_ROOT}"
    COMMENT "AOT compiling double3cpp_demo.das"
)
set(DOUBLE3CPP_AOT_GENERATED_SRC "${DOUBLE3CPP_AOT_OUTPUT}")
add_custom_target(double3cpp_aot_sandbox_dasAotStub DEPENDS "${DOUBLE3CPP_AOT_OUTPUT}")

add_executable(double3cpp_aot_sandbox
    src/main.cpp
    src/double3_module.cpp
    ${DOUBLE3CPP_AOT_GENERATED_SRC}
)

target_compile_definitions(double3cpp_aot_sandbox PRIVATE
    DAS_MOD_EXPORTS
    HELLO_DASLANG_REPO_ROOT="${HELLO_DASLANG_REPO_ROOT}"
    HELLO_DASLANG_PROJECT_ROOT="${CMAKE_CURRENT_SOURCE_DIR}"
    HELLO_DASLANG_DAS_ROOT="${HELLO_DASLANG_DAS_ROOT}"
)

target_include_directories(double3cpp_aot_sandbox PRIVATE
    "${HELLO_DASLANG_DAS_ROOT}/include"
    "${CMAKE_BINARY_DIR}/vendor/daScript/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(double3cpp_aot_sandbox PRIVATE
    libDaScriptAot
    libDaScript
    Threads::Threads
)

target_compile_features(double3cpp_aot_sandbox PRIVATE cxx_std_17)

add_dependencies(double3cpp_aot_sandbox
    libDaScriptAot
    double3cpp_aot_sandbox_dasAotStub
)

add_dependencies(double3cpp_aot_tool
    libDaScriptDyn
)

if(MSVC)
    target_compile_options(double3cpp_aot_sandbox PRIVATE /wd4005)
endif()

set_target_properties(double3cpp_aot_sandbox PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)
