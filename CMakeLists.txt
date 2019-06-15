cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_COMPILER "Visual Studio 14 2015")

if (MSVC_VERSION GREATER_EQUAL "1900")
    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("/std:c++latest" _cpp_latest_flag_supported)
    if (_cpp_latest_flag_supported)
        add_compile_options("/std:c++latest")
    endif()
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++17 -O2")
set(CMAKE_BUILD_TYPE Release)
set(HEADER_FILES YAMLEvent.h YAMLParser.h YAMLEncoding.h Common.h)

project(yamlparser CXX)

add_executable(yaml_test main.cpp ${HEADER_FILES})