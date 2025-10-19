cmake_minimum_required(VERSION 3.20)
project(${SKBUILD_PROJECT_NAME} LANGUAGES C)

find_package(
  Python
  COMPONENTS Interpreter Development.Module
  REQUIRED
)

find_package(Numpy REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS} ${NUMPY_INCLUDE_DIRS})

# Absolute path to the pyx file (no relative paths)
set(PYX_FILE "${CMAKE_CURRENT_SOURCE_DIR}/src/myproject/square.pyx")

# Generate the C file from the Cython file in the build directory
add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/square.c  # This will be in the build folder
  COMMENT
    "Making ${CMAKE_CURRENT_BINARY_DIR}/square.c from ${PYX_FILE}"
  COMMAND Python::Interpreter -m cython
          "${PYX_FILE}" --output-file ${CMAKE_CURRENT_BINARY_DIR}/square.c
  DEPENDS ${PYX_FILE}
  VERBATIM)

# Now reference the generated C file correctly in the build directory
python_add_library(square MODULE ${CMAKE_CURRENT_BINARY_DIR}/square.c WITH_SOABI)

# Ensure the module installs into the Python package directory
install(TARGETS square DESTINATION ${SKBUILD_PROJECT_NAME})
# install(TARGETS square DESTINATION ${CMAKE_BINARY_DIR}/myproject)
# install(TARGETS square DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/python${Python_VERSION_MAJOR}.${Python_VERSION_MINOR}/site-packages/myproject)


