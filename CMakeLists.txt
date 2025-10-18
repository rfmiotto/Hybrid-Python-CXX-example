cmake_minimum_required(VERSION 3.20)
project(${SKBUILD_PROJECT_NAME} LANGUAGES CXX)

set(PYBIND11_FINDPYTHON ON)
find_package(pybind11 CONFIG REQUIRED)

pybind11_add_module(core src/myproject/core.cpp)

# Optimization flags
target_compile_features(core PRIVATE cxx_std_17)
target_compile_options(core PRIVATE -O3 -march=native)

# Ensure the module installs into the Python package directory
install(TARGETS core LIBRARY DESTINATION ${SKBUILD_PROJECT_NAME})
