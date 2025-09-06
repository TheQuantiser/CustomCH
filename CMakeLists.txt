cmake_minimum_required(VERSION 3.12)
project(CombineHarvester)

# Ensure we build against the same C++ standard as ROOT to avoid
# the "C++ standard ... does not match ROOT configuration" warning.
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Default installation prefix: prefer an active Conda environment or the first
# path in CMAKE_PREFIX_PATH.  This avoids installing into /usr/local when
# working in a user environment.
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  if(DEFINED ENV{CONDA_PREFIX})
    set(CMAKE_INSTALL_PREFIX "$ENV{CONDA_PREFIX}" CACHE PATH "Install path prefix" FORCE)
  elseif(CMAKE_PREFIX_PATH)
    list(GET CMAKE_PREFIX_PATH 0 _prefix_from_path)
    set(CMAKE_INSTALL_PREFIX "${_prefix_from_path}" CACHE PATH "Install path prefix" FORCE)
  endif()
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

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

# Query ROOT for the C++ standard to use
set(_root_std "")
if(DEFINED ROOT_CXX_STANDARD)
  set(_root_std "${ROOT_CXX_STANDARD}")
elseif(DEFINED ROOT_cxx_STANDARD)
  set(_root_std "${ROOT_cxx_STANDARD}")
else()
  execute_process(COMMAND root-config --cflags
                  OUTPUT_VARIABLE _root_cflags
                  RESULT_VARIABLE _root_result
                  OUTPUT_STRIP_TRAILING_WHITESPACE)
  if(_root_result EQUAL 0)
    string(REGEX MATCH "-std=c\+\+([0-9]+)" _match "${_root_cflags}")
    if(_match)
      set(_root_std "${CMAKE_MATCH_1}")
    endif()
  endif()
endif()
if(_root_std)
  math(EXPR _root_std_int "${_root_std}")
  set(CMAKE_CXX_STANDARD ${_root_std_int})
else()
  set(CMAKE_CXX_STANDARD 17)
  message(WARNING "Could not detect C++ standard from ROOT. Falling back to C++17.")
endif()

find_package(Boost REQUIRED COMPONENTS program_options filesystem system)
find_package(LibXml2 REQUIRED)
find_package(VDT REQUIRED)
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

  install(DIRECTORY CombineTools/interface/
          DESTINATION include/CombineHarvester/CombineTools
          FILES_MATCHING PATTERN "*.h" PATTERN "*.hh")
  install(DIRECTORY CombinePdfs/interface/
          DESTINATION include/CombineHarvester/CombinePdfs
          FILES_MATCHING PATTERN "*.h" PATTERN "*.hh")
