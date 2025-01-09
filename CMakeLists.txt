cmake_minimum_required(VERSION 3.28)

# Add preprocessor definitions for the target operating system.
if(WIN32)
  add_definitions(-DWINDOWS_BUILD)
  # This prevents loguru from setting _WIN32_WINNT to an older version, which
  # effectively removes the definition of the GetProductInfo function.
  add_compile_definitions(_WIN32_WINNT=0x0600)
elseif(UNIX AND NOT APPLE)
  add_definitions(-DLINUX_BUILD)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(mmng VERSION 0.0.1)

find_package(nlohmann_json CONFIG REQUIRED)
set(nlohmann-json_IMPLICIT_CONVERSIONS OFF)
find_package(unofficial-uwebsockets CONFIG REQUIRED)

# Include subprojects.
add_subdirectory("extern/SOEM")
add_subdirectory("subprojects/loguru")

if(WIN32)
  list(APPEND MOTION_MASTER_INCLUDE_DIRECTORIES
    "${CMAKE_SOURCE_DIR}/extern/SOEM/oshw/win32/wpcap/Include")
endif()

# Set the link directories.
set(MOTION_MASTER_LINK_DIRECTORIES)
if(WIN32)
  list(APPEND MOTION_MASTER_LINK_DIRECTORIES
    ${CMAKE_SOURCE_DIR}/extern/SOEM/oshw/win32/wpcap/Lib/x64)
endif()

set(MOTION_MASTER_LINK_LIBRARIES
  loguru
  nlohmann_json::nlohmann_json
  soem
  unofficial::uwebsockets::uwebsockets)

# Winmm is required for Windows Multimedia Timers. The timer_delete function in
# Linux requires the POSIX real-time library (librt).
if(WIN32)
  list(APPEND MOTION_MASTER_LINK_LIBRARIES winmm)
elseif(UNIX AND NOT APPLE)
  list(APPEND MOTION_MASTER_LINK_LIBRARIES rt)
endif()

# Gather a list of source and header files.
file(GLOB MOTION_MASTER_SOURCE_FILES src/*.cc)
list(FILTER MOTION_MASTER_SOURCE_FILES EXCLUDE REGEX "src/.*_test\\.cc$")
file(GLOB MOTION_MASTER_HEADER_FILES src/*.h)

# Create the executable target, add the include directories, link directories,
# and link libraries.
add_executable(mmng ${MOTION_MASTER_SOURCE_FILES}
  ${MOTION_MASTER_HEADER_FILES})

target_include_directories(mmng
  PRIVATE ${MOTION_MASTER_INCLUDE_DIRECTORIES})
target_link_directories(mmng PRIVATE ${MOTION_MASTER_LINK_DIRECTORIES})
target_link_libraries(mmng PRIVATE ${MOTION_MASTER_LINK_LIBRARIES})
