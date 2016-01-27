#include "czero.h"

int main(int argc, char *argv[]) {
  auto file_path = std::string(argv[1]);
  auto ctx = ir_generator::Context("example-module");

  ir_generator::generate_example_code(ctx);
  ir_generator::print_module(file_path, ctx);
  ir_compiler::compile(file_path);
}
