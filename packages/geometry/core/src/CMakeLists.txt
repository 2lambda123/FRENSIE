FRENSIE_SETUP_PACKAGE(geometry_core
  MPI_LIBRARIES ${MPI_CXX_LIBRARIES} 
  NON_MPI_LIBRARIES ${Boost_LIBRARIES} utility_core utility_archive
  SET_VERBOSE ${CMAKE_VERBOSE_CONFIGURE})
