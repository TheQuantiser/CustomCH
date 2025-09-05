# CMake module to locate the HiggsAnalysis CombinedLimit library
#
# This module defines
#  HiggsAnalysisCombinedLimit_FOUND
#  HiggsAnalysisCombinedLimit_LIBRARIES
#  HiggsAnalysisCombinedLimit_INCLUDE_DIRS
#
# and defines an imported target
#  HiggsAnalysisCombinedLimit::HiggsAnalysisCombinedLimit

set(_HACL_HINTS "" )
if(DEFINED HiggsAnalysisCombinedLimit_ROOT)
  list(APPEND _HACL_HINTS ${HiggsAnalysisCombinedLimit_ROOT})
endif()
if(DEFINED ENV{HiggsAnalysisCombinedLimit_ROOT})
  list(APPEND _HACL_HINTS $ENV{HiggsAnalysisCombinedLimit_ROOT})
endif()

find_path(HiggsAnalysisCombinedLimit_INCLUDE_DIR
          NAMES HiggsAnalysis/CombinedLimit/interface/Combine.h
          HINTS ${_HACL_HINTS}
          PATH_SUFFIXES include)

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
