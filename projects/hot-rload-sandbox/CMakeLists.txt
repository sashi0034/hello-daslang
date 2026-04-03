add_executable(hot_rload_sandbox
    src/main.cpp
)

target_compile_definitions(hot_rload_sandbox PRIVATE
    DAS_MOD_EXPORTS
    HELLO_DASLANG_REPO_ROOT="${HELLO_DASLANG_REPO_ROOT}"
    HELLO_DASLANG_PROJECT_ROOT="${CMAKE_CURRENT_SOURCE_DIR}"
    HELLO_DASLANG_DAS_ROOT="${HELLO_DASLANG_DAS_ROOT}"
)

target_include_directories(hot_rload_sandbox PRIVATE
    "${HELLO_DASLANG_DAS_ROOT}/include"
    "${CMAKE_BINARY_DIR}/vendor/daScript/include"
)

target_link_libraries(hot_rload_sandbox PRIVATE
    libDaScript
    Threads::Threads
)

target_compile_features(hot_rload_sandbox PRIVATE cxx_std_17)

if(MSVC)
    target_compile_options(hot_rload_sandbox PRIVATE /wd4005)
endif()

set_target_properties(hot_rload_sandbox PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)
