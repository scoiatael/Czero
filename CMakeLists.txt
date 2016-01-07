cmake_minimum_required (VERSION 2.6)
project (CZero)

include(ExternalProject)
ExternalProject_Add(Maphoon
  PREFIX maphoon
  GIT_REPOSITORY git@bitbucket.org:lukaszCzaplinski/maphoon.git
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>)

add_custom_command (
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp
  COMMAND ${CMAKE_COMMAND}
  ARGS -E chdir maphoon/bin maphoon ${CMAKE_CURRENT_BINARY_DIR}/parser.m
  DEPENDS Maphoon
  )

include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

add_executable(czero czero.cpp ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp)
