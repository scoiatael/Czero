#ifndef PRETTY_PRINT_INCLUDED
#define PRETTY_PRINT_INCLUDED 1

#include "ast.hpp"
#include <iostream>
#include <cassert>
#include <boost/regex.hpp>

namespace pretty_print {
  int node(ast::abstract::Node* node);
  int value(ast::types::abstract::Value* value);
  int program(ast::Program* program);
}

#endif
