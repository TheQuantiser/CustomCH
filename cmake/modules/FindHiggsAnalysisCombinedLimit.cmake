# - Find the HiggsAnalysis CombinedLimit library
#
# This module defines the following variables:
#   HiggsAnalysisCombinedLimit_FOUND
#   HiggsAnalysisCombinedLimit_INCLUDE_DIRS
#   HiggsAnalysisCombinedLimit_LIBRARIES
#
# It also defines an imported target:
#   HiggsAnalysisCombinedLimit::HiggsAnalysisCombinedLimit
#
# The search relies on CMAKE_PREFIX_PATH and standard system prefixes.

find_path(HiggsAnalysisCombinedLimit_INCLUDE_DIR
          NAMES HiggsAnalysis/CombinedLimit/interface/Combine.h interface/Combine.h
          PATH_SUFFIXES include)

find_library(HiggsAnalysisCombinedLimit_LIBRARY
             NAMES HiggsAnalysisCombinedLimit
             PATH_SUFFIXES lib)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
  HiggsAnalysisCombinedLimit
  REQUIRED_VARS HiggsAnalysisCombinedLimit_LIBRARY HiggsAnalysisCombinedLimit_INCLUDE_DIR)

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
