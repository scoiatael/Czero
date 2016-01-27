#ifndef IR_COMPILER_INCLUDED
#define IR_COMPILER_INCLUDED    1
#include <iostream>


#include <boost/filesystem.hpp>
#include <boost/algorithm/string/join.hpp>

namespace ir_compiler {
  struct Options {
    bool remove_ir_file;
    std::string output_file_path;
  };

  int compile(std::string ir_file_path, const Options& options = Options());
}

#endif
