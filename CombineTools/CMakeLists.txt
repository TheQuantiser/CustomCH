file(GLOB COMBINE_TOOLS_SRC CONFIGURE_DEPENDS src/*.cc src/*.cpp)

get_target_property(_CL_INC HiggsAnalysisCombinedLimit::HiggsAnalysisCombinedLimit INTERFACE_INCLUDE_DIRECTORIES)
if(NOT _CL_INC AND DEFINED HiggsAnalysisCombinedLimit_SOURCE_DIR)
  set(_CL_INC "${HiggsAnalysisCombinedLimit_SOURCE_DIR}/interface")
endif()

# Build a shared library so that ROOT and CombinedLimit
# dependencies are linked once and reused by all executables.
add_library(CombineTools SHARED ${COMBINE_TOOLS_SRC})

target_compile_definitions(CombineTools PRIVATE _POSIX_C_SOURCE=200809L)

# Many sources include headers via the historic
#   "CombineHarvester/CombineTools/interface/..." path.
# Expose the repository root during the build so this
# prefix resolves to the symlinked layout created in
# the source tree.
target_include_directories(CombineTools
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/interface>
    $<INSTALL_INTERFACE:include>
  PRIVATE
    ${CH_EXTERNAL_INCLUDES}
    ${ROOT_INCLUDE_DIRS}
    ${_CL_INC}
)
target_link_libraries(CombineTools PUBLIC ${CH_EXTERNAL_LIBS} HiggsAnalysisCombinedLimit::HiggsAnalysisCombinedLimit)
set_target_properties(CombineTools PROPERTIES
  POSITION_INDEPENDENT_CODE ON
  OUTPUT_NAME CombineHarvesterCombineTools
)

file(GLOB COMBINE_TOOLS_BINS CONFIGURE_DEPENDS bin/*.cpp)
set(COMBINE_TOOLS_BINARIES)
foreach(src ${COMBINE_TOOLS_BINS})
  get_filename_component(name ${src} NAME_WE)
  add_executable(${name} ${src})
  # Link ROOT and CombinedLimit libraries explicitly to avoid
  # unresolved RooFit symbols when using the static library.
  target_link_libraries(${name} PRIVATE CombineTools ${CH_EXTERNAL_LIBS} HiggsAnalysisCombinedLimit::HiggsAnalysisCombinedLimit)
  list(APPEND COMBINE_TOOLS_BINARIES ${name})
endforeach()

install(TARGETS CombineTools ${COMBINE_TOOLS_BINARIES}
  EXPORT CombineHarvesterTargets
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  INCLUDES DESTINATION include)
