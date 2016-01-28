#include "czero.h"
#include "assert.h"

int main(int argc, char *argv[]) {
  ast::Program program;
  program.body.push_back(std::make_unique<ast::Func>());
  auto main = dynamic_cast<ast::Func*>(program.body[0].get());
  main->return_value = ast::types::Int32;
  main->body.push_back(std::make_unique<ast::Return>());
  return 0;
}
