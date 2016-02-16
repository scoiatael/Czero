#ifndef PRETTY_PRINT_INCLUDED
#define PRETTY_PRINT_INCLUDED 1

#include "ast.hpp"
#include <iostream>
#include <cassert>
#include <boost/regex.hpp>

namespace pretty_print {
  std::ostream& type(std::ostream&, const ast::types::Type);
  std::ostream& node(std::ostream&, const ast::abstract::Node&);
  std::ostream& value(std::ostream&, const ast::types::abstract::Value&);
  std::ostream& program(std::ostream&, const ast::Program&);
}

std::ostream& operator << (std::ostream&, const ast::abstract::Node&);
std::ostream& operator << (std::ostream&, const ast::types::abstract::Value&);
std::ostream& operator << (std::ostream&, const ast::Program&);

#endif
