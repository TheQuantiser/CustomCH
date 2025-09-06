# - Find the HistFactory library
#
# This module defines the following variables:
#   HistFactory_FOUND
#   HistFactory_INCLUDE_DIRS
#   HistFactory_LIBRARIES
#
# It also defines an imported target:
#   HistFactory::HistFactory
#
# The search relies on CMAKE_PREFIX_PATH and standard system prefixes.

find_path(HistFactory_INCLUDE_DIR
          NAMES RooStats/HistFactory/Measurement.h
          PATH_SUFFIXES include)

find_library(HistFactory_LIBRARY
             NAMES HistFactory
             PATH_SUFFIXES lib)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
  HistFactory
  REQUIRED_VARS HistFactory_LIBRARY HistFactory_INCLUDE_DIR)

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
