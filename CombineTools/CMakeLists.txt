file(GLOB COMBINE_TOOLS_SRC src/*.cc)
add_library(CombineTools ${COMBINE_TOOLS_SRC})

target_include_directories(CombineTools PUBLIC interface ${CH_EXTERNAL_INCLUDES})

target_link_libraries(CombineTools PUBLIC ${CH_EXTERNAL_LIBS})

set_target_properties(CombineTools PROPERTIES OUTPUT_NAME CombineHarvesterCombineTools)

install(TARGETS CombineTools
        LIBRARY DESTINATION CombineHarvester/CombineTools
        ARCHIVE DESTINATION CombineHarvester/CombineTools)

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
