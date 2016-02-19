#ifndef CHECKER_INCLUDED
#define CHECKER_INCLUDED 1

#include "ast.hpp"
#include "pretty_print.hpp"
#include <llvm/ADT/StringMap.h>

namespace checker {
  int program(ast::Program* program);
}

#endif
