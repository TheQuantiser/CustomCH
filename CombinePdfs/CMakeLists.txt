file(GLOB COMBINE_PDFS_SRC src/*.cc)

# Generate ROOT dictionary for classes in this package
get_target_property(_CL_INC HiggsAnalysisCombinedLimit::HiggsAnalysisCombinedLimit INTERFACE_INCLUDE_DIRECTORIES)
include_directories(interface ${CH_EXTERNAL_INCLUDES} ${ROOT_INCLUDE_DIRS} ${_CL_INC})
ROOT_GENERATE_DICTIONARY(G__CombinePdfs src/classes.h LINKDEF src/classes_def.xml)

add_library(CombinePdfs ${COMBINE_PDFS_SRC} ${CMAKE_CURRENT_BINARY_DIR}/G__CombinePdfs.cxx)

target_include_directories(CombinePdfs PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/interface>
  $<INSTALL_INTERFACE:include>
  ${CH_EXTERNAL_INCLUDES}
)
target_link_libraries(CombinePdfs PUBLIC CombineTools ${CH_EXTERNAL_LIBS} HiggsAnalysisCombinedLimit::HiggsAnalysisCombinedLimit)
set_target_properties(CombinePdfs PROPERTIES POSITION_INDEPENDENT_CODE ON)

file(GLOB COMBINE_PDFS_BINS bin/*.cpp)
set(COMBINE_PDFS_BINARIES)
foreach(src ${COMBINE_PDFS_BINS})
  get_filename_component(name ${src} NAME_WE)
  add_executable(${name} ${src})
  target_link_libraries(${name} CombinePdfs)
  list(APPEND COMBINE_PDFS_BINARIES ${name})
endforeach()

install(TARGETS CombinePdfs ${COMBINE_PDFS_BINARIES}
  EXPORT CombineHarvesterTargets
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)
