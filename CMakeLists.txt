cmake_minimum_required(VERSION 4.4.0)

project(tgc LANGUAGES CXX)

if(CMAKE_GENERATOR STREQUAL "Xcode")
  file(REAL_PATH "${CMAKE_SOURCE_DIR}" physical_source_directory)
  if(NOT CMAKE_SOURCE_DIR STREQUAL physical_source_directory)
    message(FATAL_ERROR
      "Xcode requires the physical source path for reliable indexing. "
      "Run 'cd \"$(pwd -P)\"' before configuring the xcode preset."
    )
  endif()
endif()

include("${CMAKE_CURRENT_SOURCE_DIR}/templates/commons.cmake")

file(
  GLOB TEMPLATE_DIRECTORIES
  CONFIGURE_DEPENDS
  LIST_DIRECTORIES true
  "${CMAKE_CURRENT_SOURCE_DIR}/templates/*"
)

foreach(template_directory IN LISTS TEMPLATE_DIRECTORIES)
  if(EXISTS "${template_directory}/CMakeLists.txt")
    add_subdirectory("${template_directory}")
  endif()
endforeach()
