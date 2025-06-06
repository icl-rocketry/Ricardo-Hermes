cmake_minimum_required(VERSION 3.13)

if(ESP_PLATFORM)
  # Build RadioLib as an ESP-IDF component
  # required because ESP-IDF runs cmake in script mode
  # and needs idf_component_register()
  file(GLOB_RECURSE RADIOLIB_ESP_SOURCES
    "src/*.*"
  )

  idf_component_register(
    SRCS ${RADIOLIB_ESP_SOURCES} 
    INCLUDE_DIRS . src 
    REQUIRES    arduino
  )

  return()
endif()

if(CMAKE_SCRIPT_MODE_FILE)
  message(FATAL_ERROR "Attempted to build RadioLib in script mode")
endif()

project(radiolib)

file(GLOB_RECURSE RADIOLIB_SOURCES
  "src/*.cpp"
)

add_library(RadioLib ${RADIOLIB_SOURCES})

target_include_directories(RadioLib
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
           $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

# use c++20 standard
set_property(TARGET RadioLib PROPERTY CXX_STANDARD 20)

# enable most warnings
target_compile_options(RadioLib PRIVATE -Wall -Wextra -Wpedantic -Wdouble-promotion)

include(GNUInstallDirs)

install(TARGETS RadioLib
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/RadioLib
  FILES_MATCHING PATTERN "*.h"
)
