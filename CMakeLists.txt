cmake_minimum_required(VERSION 3.20)
project(hello_daslang LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

find_package(Threads REQUIRED)

set(HELLO_DASLANG_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(DASCRIPT_ROOT "${HELLO_DASLANG_ROOT}/vendor/daScript")

if(NOT EXISTS "${DASCRIPT_ROOT}/CMakeLists.txt")
    message(FATAL_ERROR "vendor/daScript が見つかりません。")
endif()

set(DAS_FLEX_BISON_DISABLED ON CACHE BOOL "" FORCE)
set(DAS_CLANG_BIND_DISABLED ON CACHE BOOL "" FORCE)
set(DAS_LLVM_DISABLED ON CACHE BOOL "" FORCE)
set(DAS_HV_DISABLED ON CACHE BOOL "" FORCE)
set(DAS_GLFW_DISABLED ON CACHE BOOL "" FORCE)
set(DAS_AUDIO_DISABLED ON CACHE BOOL "" FORCE)
set(DAS_STDDLG_DISABLED ON CACHE BOOL "" FORCE)
set(DAS_STBIMAGE_DISABLED ON CACHE BOOL "" FORCE)
set(DAS_PUGIXML_DISABLED ON CACHE BOOL "" FORCE)
set(DAS_SQLITE_DISABLED ON CACHE BOOL "" FORCE)
set(DAS_TOOLS_DISABLED OFF CACHE BOOL "" FORCE)
set(DAS_TREE_SITTER_DISABLED ON CACHE BOOL "" FORCE)
set(DAS_AOT_EXAMPLES_DISABLED ON CACHE BOOL "" FORCE)
set(DAS_TUTORIAL_DISABLED ON CACHE BOOL "" FORCE)
set(DAS_TESTS_DISABLED ON CACHE BOOL "" FORCE)

add_subdirectory("${DASCRIPT_ROOT}" "${CMAKE_BINARY_DIR}/vendor/daScript")

add_executable(hello_daslang
    src/main.cpp
)

target_compile_definitions(hello_daslang PRIVATE
    DAS_MOD_EXPORTS
    HELLO_DASLANG_PROJECT_ROOT="${HELLO_DASLANG_ROOT}"
    HELLO_DASLANG_DAS_ROOT="${DASCRIPT_ROOT}"
)

target_include_directories(hello_daslang PRIVATE
    "${DASCRIPT_ROOT}/include"
    "${CMAKE_BINARY_DIR}/vendor/daScript/include"
)

target_link_libraries(hello_daslang PRIVATE
    libDaScript
    Threads::Threads
)

target_compile_features(hello_daslang PRIVATE cxx_std_17)

if(MSVC)
    target_compile_options(hello_daslang PRIVATE /wd4005)
endif()

set_target_properties(hello_daslang PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${HELLO_DASLANG_ROOT}"
)
