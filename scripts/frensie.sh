#!/bin/bash
##---------------------------------------------------------------------------##
## CONFIGURE FACEMC with CMAKE
##---------------------------------------------------------------------------##

EXTRA_ARGS=$@
HDF5_PREFIX_PATH=
MOAB_PREFIX_PATH=
DAGMC_PREFIX_PATH=
BOOST_PREFIX_PATH=
MPI_PREFIX_PATH=
ROOT_PREFIX_PATH=
SWIG_PREFIX_PATH=
DOXYGEN_PREFIX_PATH=
FRENSIE_SRC=
FRENSIE_INSTALL=
XSDIR_PATH=

# Get system details for dashboard
DISTRO="$(lsb_release -i -s)"
VERSION="$(lsb_release -r -s)"
DISTRO_VERSION=${DISTRO}-${VERSION}

source ~/.bashrc

cmake --version

cmake \
    -D CMAKE_INSTALL_PREFIX:PATH=$FRENSIE_INSTALL \
    -D CMAKE_BUILD_TYPE:STRING=RELEASE \
    -D CMAKE_VERBOSE_CONFIGURE:BOOL=OFF \
    -D CMAKE_VERBOSE_MAKEFILE:BOOL=ON \
    -D FRENSIE_ENABLE_DBC:BOOL=ON \
    -D FRENSIE_ENABLE_COLOR_OUTPUT:BOOL=ON \
    -D FRENSIE_ENABLE_OPENMP:BOOL=ON \
    -D FRENSIE_ENABLE_MPI:BOOL=ON \
    -D FRENSIE_ENABLE_MOAB:BOOL=ON \
    -D FRENSIE_ENABLE_DAGMC:BOOL=ON \
    -D FRENSIE_ENABLE_ROOT:BOOL=ON \
    -D FRENSIE_ENABLE_PROFILING:BOOL=OFF \
    -D FRENSIE_ENABLE_COVERAGE:BOOL=OFF \
    -D FRENSIE_ENABLE_EXPLICIT_TEMPLATE_INST:BOOL=ON \
    -D ROOT_PREFIX:PATH=$ROOT_PREFIX_PATH \
    -D MOAB_PREFIX:PATH=$MOAB_PREFIX_PATH \
    -D DAGMC_PREFIX:PATH=$DAGMC_PREFIX_PATH \
    -D HDF5_PREFIX:PATH=$HDF5_PREFIX_PATH \
    -D BOOST_PREFIX:PATH=$BOOST_PREFIX_PATH \
    -D MPI_PREFIX:PATH=$MPI_PREFIX_PATH \
    -D SWIG_PREFIX:PATH=$SWIG_PREFIX_PATH \
    -D DOXYGEN_PREFIX:PATH=$DOXYGEN_PREFIX_PATH \
    -D FRENSIE_ENABLE_DASHBOARD_CLIENT:BOOL=ON \
    -D XSDIR:PATH=$XSDIR_PATH \
    -D BUILDNAME_PREFIX:STRING=$DISTRO_VERSION \
    $EXTRA_ARGS \
    $FRENSIE_SRC


