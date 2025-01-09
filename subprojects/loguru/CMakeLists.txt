add_library(loguru STATIC loguru.cc loguru.h)

target_link_libraries(loguru PUBLIC)

target_include_directories(loguru PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
