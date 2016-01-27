#include "czero.h"

int main(int argc, char *argv[]) {
  auto file_path = std::string(argv[1]);
  ir_compiler::compile(file_path);
}
