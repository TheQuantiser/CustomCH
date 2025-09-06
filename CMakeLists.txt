cmake_minimum_required(VERSION 3.15)

project(CombineHarvester VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ROOT REQUIRED COMPONENTS Hist Tree RooFit RooFitCore RooStats)
find_package(Boost REQUIRED COMPONENTS program_options filesystem regex)

include(GNUInstallDirs)

add_subdirectory(CombineTools)
add_subdirectory(CombinePdfs)

install(EXPORT CombineHarvesterTargets
        NAMESPACE CombineHarvester::
        FILE CombineHarvesterTargets.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/CombineHarvester)

