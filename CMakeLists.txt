cmake_minimum_required(VERSION 4.4.0)

project(tgc LANGUAGES CXX)

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
