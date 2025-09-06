cmake_minimum_required(VERSION 3.15)

file(GLOB source_files src/*.cc)
file(GLOB header_files interface/*.h interface/*.hpp interface/*.hh)

root_generate_dictionary(G__CombineTools ${header_files} LINKDEF src/classes_def.xml)

add_library(CombineTools SHARED
    ${source_files}
    ${CMAKE_CURRENT_BINARY_DIR}/G__CombineTools.cxx)

set_target_properties(CombineTools PROPERTIES OUTPUT_NAME CombineTools)

target_include_directories(CombineTools
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/interface>
        $<INSTALL_INTERFACE:include/CombineHarvester/CombineTools>
    PRIVATE ${ROOT_INCLUDE_DIRS})

target_link_libraries(CombineTools
    PUBLIC ROOT::Core ROOT::Hist ROOT::Tree ROOT::MathCore ROOT::RooFitCore ROOT::RooFit ROOT::RooStats
           Boost::program_options Boost::filesystem Boost::regex)

install(TARGETS CombineTools EXPORT CombineHarvesterTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(DIRECTORY interface/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/CombineHarvester/CombineTools)

file(GLOB BIN_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} bin/*.cpp)
foreach(src ${BIN_SOURCES})
    get_filename_component(name ${src} NAME_WE)
    add_executable(${name} ${src})
    target_link_libraries(${name} PRIVATE CombineTools)
    install(TARGETS ${name} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endforeach()

