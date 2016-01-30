#ifndef PRETTY_PRINT_INCLUDED
#define PRETTY_PRINT_INCLUDED 1

#include "ast.hpp"

namespace pretty_print {
  int node(const ast::abstract::Node* node);
  int value(const ast::types::abstract::Value* value);
}

#endif
