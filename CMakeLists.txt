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

include("${CMAKE_CURRENT_SOURCE_DIR}/games/commons.cmake")

file(
  GLOB GAME_DIRECTORIES
  CONFIGURE_DEPENDS
  LIST_DIRECTORIES true
  "${CMAKE_CURRENT_SOURCE_DIR}/games/*"
)

foreach(template_directory IN LISTS GAME_DIRECTORIES)
  if(EXISTS "${template_directory}/CMakeLists.txt")
    add_subdirectory("${template_directory}")
  endif()
endforeach()
