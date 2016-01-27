#include "ir_generator.hpp"

ir_generator::Context::Context(std::unique_ptr<llvm::Module>& currentModule) {
  this->currentModule = std::move(currentModule);
}
