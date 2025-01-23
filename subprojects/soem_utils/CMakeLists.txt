add_library(soem_utils STATIC soem_utils.cc soem_utils.h)

target_include_directories(soem_utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(soem_utils PUBLIC soem)
