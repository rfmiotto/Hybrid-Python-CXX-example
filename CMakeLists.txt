cmake_minimum_required(VERSION 3.20)
project(${SKBUILD_PROJECT_NAME} LANGUAGES CXX)

set(PYBIND11_FINDPYTHON ON)
find_package(pybind11 CONFIG REQUIRED)

# Try to find Eigen3 (system)
find_package(Eigen3 QUIET)

# If not found, fetch it
if (NOT Eigen3_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        eigen
        GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
        GIT_TAG 3.4.0
    )
    FetchContent_MakeAvailable(eigen)
endif()

pybind11_add_module(core src/myproject/core.cpp)

target_link_libraries(core PRIVATE pybind11::module Eigen3::Eigen)

# Optimization flags
target_compile_features(core PRIVATE cxx_std_17)
target_compile_options(core PRIVATE -O3 -march=native)

install(TARGETS core LIBRARY DESTINATION ${SKBUILD_PROJECT_NAME})
