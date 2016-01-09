cmake_minimum_required (VERSION 2.6)
project (CZero)

set(lib ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(maphoon_root ${CMAKE_CURRENT_BINARY_DIR}/maphoon)
set(bin ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(parser ${CMAKE_CURRENT_SOURCE_DIR}/parser)

set(parser_files
  parser.cpp
  parser.h
  token.cpp
  token.h)

set(source
  czero.cpp
  ${parser}/tokenizer.cpp)

set(maphoon_files "")
foreach(file ${parser_files})
  list(APPEND maphoon_files ${maphoon_root}/bin/${file})
endforeach(file)

set(final_parser_files "")
foreach(file ${parser_files})
  list(APPEND final_parser_files ${parser}/${file})
endforeach(file)

include(ExternalProject)
ExternalProject_Add(Maphoon
  PREFIX maphoon
  GIT_REPOSITORY git@bitbucket.org:lukaszCzaplinski/maphoon.git
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>)

add_custom_command(
  OUTPUT ${maphoon_files}
  COMMAND ./maphoon ${parser}/parser.m
  DEPENDS Maphoon
  DEPENDS ${parser}/parser.m
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

add_executable(czero ${source} ${final_parser_files})
