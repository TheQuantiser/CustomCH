cmake_minimum_required(VERSION 3.12)
project(CombineHarvester)

# Ensure we build against the same C++ standard as ROOT to avoid
# the "C++ standard ... does not match ROOT configuration" warning.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

option(USE_SYSTEM_COMBINEDLIMIT "Use system-installed HiggsAnalysisCombinedLimit" OFF)
set(COMBINEDLIMIT_TAG "main" CACHE STRING "Git branch or tag of HiggsAnalysis-CombinedLimit to fetch")

if(USE_SYSTEM_COMBINEDLIMIT)
  find_package(HiggsAnalysisCombinedLimit REQUIRED)
else()
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/HiggsAnalysis/CombinedLimit/CMakeLists.txt")
    add_subdirectory(HiggsAnalysis/CombinedLimit)
    set(HiggsAnalysisCombinedLimit_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/HiggsAnalysis/CombinedLimit)
  else()
    include(FetchContent)
    FetchContent_Declare(CombinedLimit
      GIT_REPOSITORY https://github.com/cms-analysis/HiggsAnalysis-CombinedLimit.git
      GIT_TAG        ${COMBINEDLIMIT_TAG}
    )
    FetchContent_MakeAvailable(CombinedLimit)
    set(HiggsAnalysisCombinedLimit_SOURCE_DIR ${combinedlimit_SOURCE_DIR})
  endif()
  add_library(HiggsAnalysisCombinedLimit::HiggsAnalysisCombinedLimit ALIAS HiggsAnalysisCombinedLimit)
endif()

find_package(ROOT REQUIRED COMPONENTS RooFitCore RooFit RooStats)
find_package(Boost REQUIRED COMPONENTS program_options filesystem system)
find_package(LibXml2 REQUIRED)
find_package(vdt REQUIRED)
find_package(HistFactory REQUIRED)

set(CH_EXTERNAL_INCLUDES
  ${ROOT_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
  ${LIBXML2_INCLUDE_DIR}
)

set(CH_EXTERNAL_LIBS
  ROOT::RooFitCore
  ROOT::RooFit
  ROOT::RooStats
  Boost::program_options
  Boost::filesystem
  Boost::system
  ${LIBXML2_LIBRARIES}
  vdt::vdt
  HistFactory::HistFactory
)

add_subdirectory(CombineTools)
add_subdirectory(CombinePdfs)

add_library(CombineHarvester INTERFACE)
target_include_directories(CombineHarvester INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/CombineTools/interface>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/CombinePdfs/interface>
  $<INSTALL_INTERFACE:include>
)

target_link_libraries(CombineHarvester INTERFACE
  CombineTools
  CombinePdfs
  HiggsAnalysisCombinedLimit::HiggsAnalysisCombinedLimit)

if(NOT USE_SYSTEM_COMBINEDLIMIT)
  install(DIRECTORY ${HiggsAnalysisCombinedLimit_SOURCE_DIR}/interface/
          DESTINATION include/HiggsAnalysis/CombinedLimit
          FILES_MATCHING PATTERN "*.h" PATTERN "*.hh")
endif()

install(TARGETS CombineHarvester EXPORT CombineHarvesterTargets)
install(EXPORT CombineHarvesterTargets
        FILE CombineHarvesterTargets.cmake
        NAMESPACE CombineHarvester::
        DESTINATION lib/cmake/CombineHarvester)
