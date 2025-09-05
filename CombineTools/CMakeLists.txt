file(GLOB COMBINE_TOOLS_SRC src/*.cc)

# Generate ROOT dictionary for classes in this package
include_directories(interface ${CH_EXTERNAL_INCLUDES} ${ROOT_INCLUDE_DIRS})
ROOT_GENERATE_DICTIONARY(G__CombineTools src/classes.h LINKDEF src/classes_def.xml)

add_library(CombineTools ${COMBINE_TOOLS_SRC} ${CMAKE_CURRENT_BINARY_DIR}/G__CombineTools.cxx)

target_include_directories(CombineTools PUBLIC interface ${CH_EXTERNAL_INCLUDES})
target_link_libraries(CombineTools PUBLIC ${CH_EXTERNAL_LIBS})
set_target_properties(CombineTools PROPERTIES POSITION_INDEPENDENT_CODE ON)

file(GLOB COMBINE_TOOLS_BINS bin/*.cpp)
set(COMBINE_TOOLS_BINARIES)
foreach(src ${COMBINE_TOOLS_BINS})
  get_filename_component(name ${src} NAME_WE)
  add_executable(${name} ${src})
  target_link_libraries(${name} CombineTools)
  list(APPEND COMBINE_TOOLS_BINARIES ${name})
endforeach()

install(TARGETS CombineTools ${COMBINE_TOOLS_BINARIES}
  EXPORT CombineHarvesterTargets
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)
