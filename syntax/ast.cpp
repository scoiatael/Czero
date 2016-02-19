#include "ast.hpp"

int ast::generate_new_id()
{
  static int static_counter = 0;
  return static_counter++;
}
