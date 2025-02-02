cmake_minimum_required(VERSION 3.23)

project(libmeta CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    message(STATUS "using clang compiler")
    add_compile_options(-fvisibility=hidden)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message(STATUS "using GCC compiler")
    add_compile_options(-fvisibility=hidden)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    message(STATUS "using MSVC compiler")
endif()

add_library(libmeta_headers INTERFACE)
target_include_directories(libmeta_headers INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(src)

enable_testing()
add_subdirectory(tests)
