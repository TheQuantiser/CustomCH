# CMake module to locate the HiggsAnalysis CombinedLimit library
#
# This module defines
#  HiggsAnalysisCombinedLimit_FOUND
#  HiggsAnalysisCombinedLimit_LIBRARIES
#  HiggsAnalysisCombinedLimit_INCLUDE_DIRS
#
# and defines an imported target
#  HiggsAnalysisCombinedLimit::HiggsAnalysisCombinedLimit

find_path(HiggsAnalysisCombinedLimit_INCLUDE_DIR
          NAMES HiggsAnalysis/CombinedLimit/interface/Combine.h)

find_library(HiggsAnalysisCombinedLimit_LIBRARY
             NAMES HiggsAnalysisCombinedLimit)

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
