cmake_minimum_required(VERSION 3.8)
project(hexcrafter CXX)

# Set hard requirements for C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Warn on CMake API deprecations
set(CMAKE_WARN_DEPRECATED ON)

# Add the module directory to the list of paths
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMakeModules")

# Default to a Release build
if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING
      "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Compiler flags
if (MSVC)
    set(HEXCRAFTER_CXX_FLAGS
        /std:c++latest
        /WX
        /W4
        /permissive- # Stricter C++ standards conformance
        /MP # Parallel builds
        /Zi # Generate PDB
        /Zo # Enchance optimized debugging
        /EHsc # No structured exceptions, assume extern "C" does not throw
        /Zc:throwingNew # Assumes new never returns null
        /Zc:inline) # Omits inline functions from object-file output

    if (CMAKE_VS_PLATFORM_TOOLSET MATCHES "LLVM-vs[0-9]+")
        list(APPEND HEXCRAFTER_CXX_FLAGS
             -Qunused-arguments
             -Wno-missing-braces)
    endif()
else()
    set(HEXCRAFTER_CXX_FLAGS
        -fcoroutines-ts 
        -Werror
        -Wall
        -Wextra
        -Wcast-qual
        -pedantic
        -pedantic-errors
        -Wfatal-errors
        -Wno-missing-braces)
endif()

# Enable unit-testing
enable_testing(true)

# External libraries
add_subdirectory(externals)

# Project files
add_subdirectory(src)
add_subdirectory(tests)
