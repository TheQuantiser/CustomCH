cmake_minimum_required(VERSION 3.16)
project(CombineHarvester)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ROOT REQUIRED COMPONENTS RooFit RooStats HistFactory)
find_package(Boost REQUIRED COMPONENTS program_options filesystem regex)
find_package(LibXml2 REQUIRED)

find_library(VDT_LIBRARY vdt)

set(CH_EXTERNAL_LIBS ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${LIBXML2_LIBRARIES})
if(VDT_LIBRARY)
  list(APPEND CH_EXTERNAL_LIBS ${VDT_LIBRARY})
endif()

set(CH_EXTERNAL_INCLUDES ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${LIBXML2_INCLUDE_DIR})

add_subdirectory(CombineTools)
add_subdirectory(CombinePdfs)
