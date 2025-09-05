cmake_minimum_required(VERSION 3.12)
project(CombineHarvester)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

option(USE_SYSTEM_COMBINEDLIMIT "Use system-installed HiggsAnalysisCombinedLimit" OFF)
set(COMBINEDLIMIT_TAG "v10.2.7" CACHE STRING "Git tag or commit of HiggsAnalysis-CombinedLimit to fetch")

if(USE_SYSTEM_COMBINEDLIMIT)
  find_package(HiggsAnalysisCombinedLimit REQUIRED)
else()
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/HiggsAnalysis/CombinedLimit/CMakeLists.txt")
    add_subdirectory(HiggsAnalysis/CombinedLimit)
    set(HiggsAnalysisCombinedLimit_LIBRARIES HiggsAnalysisCombinedLimit)
    set(HiggsAnalysisCombinedLimit_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/HiggsAnalysis/CombinedLimit/interface)
  else()
    include(FetchContent)
    FetchContent_Declare(CombinedLimit
      GIT_REPOSITORY https://github.com/cms-analysis/HiggsAnalysis-CombinedLimit.git
      GIT_TAG        ${COMBINEDLIMIT_TAG}
    )
    FetchContent_MakeAvailable(CombinedLimit)
    set(HiggsAnalysisCombinedLimit_LIBRARIES HiggsAnalysisCombinedLimit)
    set(HiggsAnalysisCombinedLimit_INCLUDE_DIRS ${combinedlimit_SOURCE_DIR}/interface)
  endif()
endif()

find_package(ROOT REQUIRED COMPONENTS RooFit RooStats)
find_package(Boost REQUIRED)
find_package(LibXml2 REQUIRED)
find_package(vdt REQUIRED)
find_package(HistFactory REQUIRED)

set(CH_EXTERNAL_INCLUDES
  ${ROOT_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
  ${LIBXML2_INCLUDE_DIR}
)

set(CH_EXTERNAL_LIBS
  ${ROOT_LIBRARIES}
  ${Boost_LIBRARIES}
  ${LIBXML2_LIBRARIES}
  vdt::vdt
  HistFactory::HistFactory
)

add_subdirectory(CombineTools)
add_subdirectory(CombinePdfs)

add_library(CombineHarvester INTERFACE)
target_include_directories(CombineHarvester INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/CombineTools/interface
  ${CMAKE_CURRENT_SOURCE_DIR}/CombinePdfs/interface
  ${HiggsAnalysisCombinedLimit_INCLUDE_DIRS}
)

target_link_libraries(CombineHarvester INTERFACE CombineTools CombinePdfs ${HiggsAnalysisCombinedLimit_LIBRARIES})

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/HiggsAnalysis/CombinedLimit/interface/
        DESTINATION include/HiggsAnalysis/CombinedLimit
        FILES_MATCHING PATTERN "*.h" PATTERN "*.hh")

install(TARGETS CombineHarvester EXPORT CombineHarvesterTargets)
install(EXPORT CombineHarvesterTargets
        FILE CombineHarvesterTargets.cmake
        NAMESPACE CombineHarvester::
        DESTINATION lib/cmake/CombineHarvester)
