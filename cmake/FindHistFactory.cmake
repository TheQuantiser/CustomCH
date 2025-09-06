# CMake module to locate the HistFactory library
#
# This module defines
#  HistFactory_FOUND
#  HistFactory_LIBRARIES
#  HistFactory_INCLUDE_DIRS
#
# and defines an imported target
#  HistFactory::HistFactory

find_path(HistFactory_INCLUDE_DIR
          NAMES RooStats/HistFactory/Measurement.h
          HINTS $ENV{ROOTSYS}/include)

find_library(HistFactory_LIBRARY
             NAMES HistFactory
             HINTS $ENV{ROOTSYS}/lib $ENV{ROOTSYS}/lib64)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    HistFactory
    DEFAULT_MSG
    HistFactory_INCLUDE_DIR
    HistFactory_LIBRARY)

if(HistFactory_FOUND)
  set(HistFactory_LIBRARIES ${HistFactory_LIBRARY})
  set(HistFactory_INCLUDE_DIRS ${HistFactory_INCLUDE_DIR})
  if(NOT TARGET HistFactory::HistFactory)
    add_library(HistFactory::HistFactory UNKNOWN IMPORTED)
    set_target_properties(HistFactory::HistFactory PROPERTIES
      IMPORTED_LOCATION "${HistFactory_LIBRARY}"
      INTERFACE_INCLUDE_DIRECTORIES "${HistFactory_INCLUDE_DIR}")
  endif()
endif()

