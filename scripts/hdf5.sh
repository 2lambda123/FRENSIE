#!/bin/bash

EXTRA_ARGS=$@

HDF5_DIR=$HOME/hdf5-path

source $HOME/.bashrc

cmake --version

cmake \
-D CMAKE_INSTALL_PREFIX:PATH=$HDF5_DIR \
-D CMAKE_BUILD_TYPE:STRING=RELEASE \
-D CMAKE_VERBOSE_MAKEFILE:BOOL=ON  \
-D HDF5_ENABLE_Z_LIB_SUPPORT:BOOL=OFF \
-D HDF5_ENABLE_USING_MEMCHECKER:BOOL=ON \
-D BUILD_SHARED_LIBS=OFF \
-D BUILD_STATIC_EXECS:BOOL=OFF \
-D BUILD_TESTING:BOOL=ON \
-D HDF5_BUILD_EXAMPLES:BOOL=ON \
-D HDF5_BUILD_CPP_LIB:BOOL=ON \
-D HDF5_BUILD_TOOLS:BOOL=ON \
-D HDF5_BUILD_HL_LIB:BOOL=ON \
$EXTRA_ARGS \
$HDF5_DIR/src