cmake_minimum_required(VERSION 3.15)

file(GLOB source_files src/*.cc)
file(GLOB header_files interface/*.h interface/*.hpp interface/*.hh)

root_generate_dictionary(G__CombinePdfs ${header_files} LINKDEF src/classes_def.xml)

add_library(CombinePdfs SHARED
    ${source_files}
    ${CMAKE_CURRENT_BINARY_DIR}/G__CombinePdfs.cxx)

set_target_properties(CombinePdfs PROPERTIES OUTPUT_NAME CombinePdfs)

target_include_directories(CombinePdfs
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/interface>
        $<INSTALL_INTERFACE:include/CombineHarvester/CombinePdfs>
    PRIVATE ${ROOT_INCLUDE_DIRS})

target_link_libraries(CombinePdfs
    PUBLIC CombineTools ROOT::Core ROOT::Hist ROOT::Tree ROOT::MathCore ROOT::RooFitCore ROOT::RooFit ROOT::RooStats)

install(TARGETS CombinePdfs EXPORT CombineHarvesterTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(DIRECTORY interface/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/CombineHarvester/CombinePdfs)

file(GLOB BIN_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} bin/*.cpp)
foreach(src ${BIN_SOURCES})
    get_filename_component(name ${src} NAME_WE)
    add_executable(${name} ${src})
    target_link_libraries(${name} PRIVATE CombinePdfs)
    install(TARGETS ${name} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endforeach()

