file(GLOB CONFIGURE_DEPENDS COMBINE_PDFS_SRC src/*.cc)

get_target_property(_CL_INC HiggsAnalysisCombinedLimit::HiggsAnalysisCombinedLimit INTERFACE_INCLUDE_DIRECTORIES)

# Build a shared library so that ROOT dependencies are linked once.
add_library(CombinePdfs SHARED ${COMBINE_PDFS_SRC})

# Allow includes of the form
#   "CombineHarvester/CombineTools/interface/..."
# by exposing the repository root during the build.
target_include_directories(CombinePdfs PUBLIC
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/interface>
  $<INSTALL_INTERFACE:include>
  ${CH_EXTERNAL_INCLUDES}
  ${ROOT_INCLUDE_DIRS}
  ${_CL_INC}
)
target_link_libraries(CombinePdfs PUBLIC CombineTools ${CH_EXTERNAL_LIBS} HiggsAnalysisCombinedLimit::HiggsAnalysisCombinedLimit)
set_target_properties(CombinePdfs PROPERTIES POSITION_INDEPENDENT_CODE ON)

file(GLOB CONFIGURE_DEPENDS COMBINE_PDFS_BINS bin/*.cpp)
set(COMBINE_PDFS_BINARIES)
foreach(src ${COMBINE_PDFS_BINS})
  get_filename_component(name ${src} NAME_WE)
  add_executable(${name} ${src})
  target_link_libraries(${name} PRIVATE CombinePdfs ${CH_EXTERNAL_LIBS} HiggsAnalysisCombinedLimit::HiggsAnalysisCombinedLimit)
  list(APPEND COMBINE_PDFS_BINARIES ${name})
endforeach()

install(TARGETS CombinePdfs ${COMBINE_PDFS_BINARIES}
  EXPORT CombineHarvesterTargets
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  INCLUDES DESTINATION include)
