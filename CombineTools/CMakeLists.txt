file(GLOB COMBINE_TOOLS_SRC src/*.cc)
add_library(CombineTools ${COMBINE_TOOLS_SRC})

target_include_directories(CombineTools PUBLIC interface ${CH_EXTERNAL_INCLUDES})

target_link_libraries(CombineTools PUBLIC ${CH_EXTERNAL_LIBS})

file(GLOB COMBINE_TOOLS_BINS bin/*.cpp)
foreach(src ${COMBINE_TOOLS_BINS})
  get_filename_component(name ${src} NAME_WE)
  add_executable(${name} ${src})
  target_link_libraries(${name} CombineTools)
endforeach()
