# CMake module to locate the HiggsAnalysis CombinedLimit library
#
# This module defines
#  HiggsAnalysisCombinedLimit_FOUND
#  HiggsAnalysisCombinedLimit_LIBRARIES
#  HiggsAnalysisCombinedLimit_INCLUDE_DIRS
#
# and defines an imported target
#  HiggsAnalysisCombinedLimit::HiggsAnalysisCombinedLimit

set(_HACL_HINTS "")
if(DEFINED HiggsAnalysisCombinedLimit_ROOT)
  list(APPEND _HACL_HINTS ${HiggsAnalysisCombinedLimit_ROOT})
endif()
if(DEFINED ENV{HiggsAnalysisCombinedLimit_ROOT})
  list(APPEND _HACL_HINTS $ENV{HiggsAnalysisCombinedLimit_ROOT})
endif()

# Common checkout layout places the CombinedLimit repository as a sibling of
# CombineHarvester:  ../HiggsAnalysis/CombinedLimit.  Fall back to searching
# these locations (and their build directories) when the user does not provide
# an explicit hint.
get_filename_component(_hacl_module_dir "${CMAKE_CURRENT_LIST_DIR}" ABSOLUTE)
list(APPEND _HACL_HINTS
  "${_hacl_module_dir}/../HiggsAnalysis/CombinedLimit/build"
  "${_hacl_module_dir}/../HiggsAnalysis/CombinedLimit"
  "${_hacl_module_dir}/../../HiggsAnalysis/CombinedLimit/build"
  "${_hacl_module_dir}/../../HiggsAnalysis/CombinedLimit")

list(REMOVE_DUPLICATES _HACL_HINTS)

# Search header hints in both the provided directories and their parents to
# account for a source tree checked out as HiggsAnalysis/CombinedLimit.
set(_HACL_HEADER_HINTS ${_HACL_HINTS})
foreach(_dir IN LISTS _HACL_HINTS)
  get_filename_component(_parent "${_dir}/.." ABSOLUTE)
  list(APPEND _HACL_HEADER_HINTS ${_parent})
endforeach()
list(REMOVE_DUPLICATES _HACL_HEADER_HINTS)

find_path(HiggsAnalysisCombinedLimit_INCLUDE_DIR
          NAMES HiggsAnalysis/CombinedLimit/interface/Combine.h interface/Combine.h
          HINTS ${_HACL_HEADER_HINTS}
          PATH_SUFFIXES include .)

find_library(HiggsAnalysisCombinedLimit_LIBRARY
             NAMES HiggsAnalysisCombinedLimit
             HINTS ${_HACL_HINTS}
             PATH_SUFFIXES lib build/lib)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    HiggsAnalysisCombinedLimit
    DEFAULT_MSG
    HiggsAnalysisCombinedLimit_INCLUDE_DIR
    HiggsAnalysisCombinedLimit_LIBRARY)

if(HiggsAnalysisCombinedLimit_FOUND)
  set(HiggsAnalysisCombinedLimit_LIBRARIES ${HiggsAnalysisCombinedLimit_LIBRARY})
  set(HiggsAnalysisCombinedLimit_INCLUDE_DIRS ${HiggsAnalysisCombinedLimit_INCLUDE_DIR})
  if(NOT TARGET HiggsAnalysisCombinedLimit::HiggsAnalysisCombinedLimit)
    add_library(HiggsAnalysisCombinedLimit::HiggsAnalysisCombinedLimit UNKNOWN IMPORTED)
    set_target_properties(HiggsAnalysisCombinedLimit::HiggsAnalysisCombinedLimit PROPERTIES
      IMPORTED_LOCATION "${HiggsAnalysisCombinedLimit_LIBRARY}"
      INTERFACE_INCLUDE_DIRECTORIES "${HiggsAnalysisCombinedLimit_INCLUDE_DIR}")
  endif()
endif()
