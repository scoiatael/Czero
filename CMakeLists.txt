cmake_minimum_required (VERSION 3.0)
project (CZero)

set( CMAKE_EXPORT_COMPILE_COMMANDS 1 )

set(lib ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(maphoon_root ${CMAKE_CURRENT_BINARY_DIR}/maphoon)
set(bin ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(parser ${CMAKE_CURRENT_SOURCE_DIR}/parser)
set(util ${CMAKE_CURRENT_SOURCE_DIR}/util)
set(syntax ${CMAKE_CURRENT_SOURCE_DIR}/syntax)
set(build ${CMAKE_CURRENT_SOURCE_DIR}/build)

set(parser_files
  parser.cpp
  parser.h
  token.cpp
  token.h)

set(source
  ${CMAKE_CURRENT_SOURCE_DIR}/czero.cpp
  ${syntax}/ast.cpp
  ${syntax}/pretty_print.cpp
  ${syntax}/checker.cpp
  ${syntax}/compiler.cpp
  ${util}/ir_compiler.cpp
  ${util}/ir_generator.cpp
  ${parser}/varstore.cpp
  ${parser}/tokenizer.cpp)

set(maphoon_files "")
foreach(file ${parser_files})
  list(APPEND maphoon_files ${maphoon_root}/bin/${file})
endforeach(file)

set(final_parser_files "")
foreach(file ${parser_files})
  list(APPEND final_parser_files ${parser}/${file})
endforeach(file)

find_package( Boost 1.40 COMPONENTS program_options filesystem system regex REQUIRED )
include_directories( ${Boost_INCLUDE_DIR} )


find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
llvm_map_components_to_libnames(llvm_libs support core irreader)
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

include(ExternalProject)
ExternalProject_Add(Maphoon
  UPDATE_COMMAND ""
  PREFIX maphoon
  GIT_REPOSITORY git@bitbucket.org:lukaszCzaplinski/maphoon.git
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>)

add_custom_command(
  OUTPUT ${maphoon_files}
  COMMAND ./maphoon ${parser}/parser.m
  DEPENDS Maphoon
  DEPENDS ${parser}/parser.m ${parser}/varstore.h
  WORKING_DIRECTORY ${maphoon_root}/bin)

foreach(index 0 1 2 3)
  list(GET maphoon_files ${index} src)
  list(GET final_parser_files ${index} dest)

  add_custom_command(
    OUTPUT ${dest}
    COMMAND cp ${src} ${dest}
    DEPENDS ${src})
endforeach(index)

include_directories(AFTER SYSTEM ${parser})

add_custom_target(extra-clean
                   COMMAND rm -rf ${build} && mkdir ${build} && touch ${build}/.keep )

add_executable(czero ${source} ${final_parser_files})

set_property(TARGET czero PROPERTY CXX_STANDARD 14)
set_property(TARGET czero PROPERTY CXX_STANDARD_REQUIRED ON)

target_link_libraries(czero ${Boost_LIBRARIES} )

target_link_libraries(czero ${llvm_libs})
