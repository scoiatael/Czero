#ifndef COMPILER_INCLUDED
#define COMPILER_INCLUDED 1

#include "../util/ir_generator.hpp"
#include "ast.hpp"
#include <llvm/ADT/StringMap.h>

namespace compiler {
  int program(ast::Program* program, ir_generator::Context& context);
}

#endif
