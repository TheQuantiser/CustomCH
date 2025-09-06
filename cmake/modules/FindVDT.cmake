# - Find the VDT library
#
# This module defines the following variables:
#   VDT_FOUND
#   VDT_INCLUDE_DIRS
#   VDT_LIBRARIES
#
# It also defines an imported target:
#   VDT::VDT
#
# The search relies on CMAKE_PREFIX_PATH and standard system prefixes.

find_path(VDT_INCLUDE_DIR
          NAMES vdt/VDTMath.h vdt/vdtMath.h
          PATH_SUFFIXES include)

find_library(VDT_LIBRARY
             NAMES VDT vdt
             PATH_SUFFIXES lib)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
  VDT
  REQUIRED_VARS VDT_LIBRARY VDT_INCLUDE_DIR)

if(VDT_FOUND)
  set(VDT_LIBRARIES ${VDT_LIBRARY})
  set(VDT_INCLUDE_DIRS ${VDT_INCLUDE_DIR})
  if(NOT TARGET VDT::VDT)
    add_library(VDT::VDT UNKNOWN IMPORTED)
    set_target_properties(VDT::VDT PROPERTIES
      IMPORTED_LOCATION "${VDT_LIBRARY}"
      INTERFACE_INCLUDE_DIRECTORIES "${VDT_INCLUDE_DIR}")
  endif()
endif()
