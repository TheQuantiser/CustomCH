# CMake module to locate the vdt library
#
# This module defines
#  vdt_FOUND
#  vdt_LIBRARIES
#  vdt_INCLUDE_DIRS
#
# and defines an imported target
#  vdt::vdt

find_path(vdt_INCLUDE_DIR
          NAMES vdt/vdtMath.h
          HINTS $ENV{ROOTSYS}/include)

find_library(vdt_LIBRARY
             NAMES vdt
             HINTS $ENV{ROOTSYS}/lib $ENV{ROOTSYS}/lib64)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    vdt
    DEFAULT_MSG
    vdt_INCLUDE_DIR
    vdt_LIBRARY)

if(vdt_FOUND)
  set(vdt_LIBRARIES ${vdt_LIBRARY})
  set(vdt_INCLUDE_DIRS ${vdt_INCLUDE_DIR})
  if(NOT TARGET vdt::vdt)
    add_library(vdt::vdt UNKNOWN IMPORTED)
    set_target_properties(vdt::vdt PROPERTIES
      IMPORTED_LOCATION "${vdt_LIBRARY}"
      INTERFACE_INCLUDE_DIRECTORIES "${vdt_INCLUDE_DIR}")
  endif()
endif()

