cmake_minimum_required(VERSION 3.15)
project(reicore VERSION 2.0.0 LANGUAGES CXX)

# ============================================================================
# C++ Standard and Build Configuration
# ============================================================================

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type default
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# ============================================================================
# Compiler Options
# ============================================================================

if(MSVC)
    # MSVC
    add_compile_options(/W4 /O2 /arch:AVX2 /DNDEBUG)
else()
    # GCC/Clang
    add_compile_options(
        -Wall -Wextra -Wpedantic
        -O3
        -march=native
        -DNDEBUG
        -ffast-math
        -funroll-loops
    )
endif()

# ============================================================================
# Options
# ============================================================================

option(BUILD_PYTHON_MODULE "Build Python module with pybind11" ON)
option(BUILD_TESTS "Build test suite" ON)
option(BUILD_BENCHMARK "Build benchmark suite" ON)
option(BUILD_EXAMPLES "Build example programs" ON)

# ============================================================================
# Header-Only Library (rei_sort.hpp)
# ============================================================================

add_library(rei_sort INTERFACE)
target_include_directories(rei_sort INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(rei_sort INTERFACE cxx_std_20)

# ============================================================================
# Python Module (pybind11)
# ============================================================================

if(BUILD_PYTHON_MODULE)
    find_package(pybind11 QUIET)

    if(pybind11_FOUND)
        pybind11_add_module(reicore reicore.cpp)
        target_link_libraries(reicore PRIVATE rei_sort)

        # Optimization flags for Python module
        if(MSVC)
            target_compile_options(reicore PRIVATE /O2 /arch:AVX2 /DNDEBUG)
        else()
            target_compile_options(reicore PRIVATE
                -O3
                -march=native
                -DNDEBUG
                -ffast-math
                -funroll-loops
                -fvisibility=hidden
            )
        endif()

        # Strip symbols in release mode
        if(CMAKE_BUILD_TYPE STREQUAL "Release")
            if(UNIX AND NOT APPLE)
                add_custom_command(TARGET reicore POST_BUILD
                    COMMAND ${CMAKE_STRIP} --strip-unneeded $<TARGET_FILE:reicore>
                )
            endif()
        endif()
    else()
        message(WARNING "pybind11 not found, skipping Python module build")
    endif()
endif()

# ============================================================================
# Test Suite
# ============================================================================

if(BUILD_TESTS)
    add_executable(rei_test test.cpp)
    target_link_libraries(rei_test PRIVATE rei_sort)

    # Add test to CTest
    enable_testing()
    add_test(NAME rei_test COMMAND rei_test)
endif()

# ============================================================================
# Benchmark Suite
# ============================================================================

if(BUILD_BENCHMARK)
    add_executable(rei_benchmark benchmark.cpp)
    target_link_libraries(rei_benchmark PRIVATE rei_sort)

    # Extra optimization for benchmarks
    if(NOT MSVC)
        target_compile_options(rei_benchmark PRIVATE -O3 -march=native -flto)
        target_link_options(rei_benchmark PRIVATE -flto)
    endif()
endif()

# ============================================================================
# Example Programs
# ============================================================================

if(BUILD_EXAMPLES)
    add_executable(rei_example example.cpp)
    target_link_libraries(rei_example PRIVATE rei_sort)
endif()

# ============================================================================
# Installation
# ============================================================================

include(GNUInstallDirs)

# Install header-only library
install(FILES rei_sort.hpp
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Install Python module (if built)
if(BUILD_PYTHON_MODULE AND pybind11_FOUND)
    install(TARGETS reicore
        LIBRARY DESTINATION .
    )
endif()

# ============================================================================
# Summary
# ============================================================================

message(STATUS "")
message(STATUS "╔════════════════════════════════════════════════════════════╗")
message(STATUS "║          REI SORT - Build Configuration                   ║")
message(STATUS "╚════════════════════════════════════════════════════════════╝")
message(STATUS "  Build type:         ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ standard:       C++${CMAKE_CXX_STANDARD}")
message(STATUS "  Compiler:           ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "  Python module:      ${BUILD_PYTHON_MODULE}")
message(STATUS "  Tests:              ${BUILD_TESTS}")
message(STATUS "  Benchmarks:         ${BUILD_BENCHMARK}")
message(STATUS "  Examples:           ${BUILD_EXAMPLES}")
message(STATUS "")
