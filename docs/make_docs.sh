#!/bin/bash -e

# Optional build directory may be specified as the first argument
BUILD_DIR=${1:-build}

# Ensure the libraries and python modules from the CMake build are found
export LD_LIBRARY_PATH="$PWD/${BUILD_DIR}/CombineTools:${LD_LIBRARY_PATH}"
export PYTHONPATH="$PWD/${BUILD_DIR}:${PYTHONPATH}"

doxygen Doxyfile
# We need a custom page resize function
# because of the modifications to the
# page layout
cp docs/resize.js docs/html/
