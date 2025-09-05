file(GLOB COMBINE_PDFS_SRC src/*.cc)
add_library(CombinePdfs ${COMBINE_PDFS_SRC})

target_include_directories(CombinePdfs PUBLIC interface ${CH_EXTERNAL_INCLUDES})

target_link_libraries(CombinePdfs PUBLIC CombineTools ${CH_EXTERNAL_LIBS})

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
