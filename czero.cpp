#include "czero.h"

void generate_example_code(ir_generator::Context& ctx) {
  auto int8t = llvm::Type::getInt8Ty(ctx.llvmContext);
  auto int8ptrt = llvm::PointerType::getUnqual(int8t);
  auto printf_args_type = std::vector<llvm::Type*>(1, int8ptrt);
  auto int32t = llvm::Type::getInt32Ty(ctx.llvmContext);

  auto *printf_type =
    llvm::FunctionType::get(int32t, printf_args_type, true);

  auto *printf_func =
    llvm::Function::Create(printf_type, llvm::Function::ExternalLinkage, "printf", ctx.currentModule.get());

  auto main_return_type = int32t;
  auto *main_func_type =
    llvm::FunctionType::get(main_return_type, false);

  auto *main_func =
    llvm::Function::Create(main_func_type, llvm::Function::ExternalLinkage, "main", ctx.currentModule.get());

  auto *block = llvm::BasicBlock::Create(ctx.llvmContext, "entry", main_func);
  ctx.Builder.SetInsertPoint(block);

  auto *helloWorld = ctx.Builder.CreateGlobalStringPtr("hello world!\n");

  auto printf_args = std::vector< llvm::Value* >(1, helloWorld);

  ctx.Builder.CreateCall(printf_func, printf_args, "calltmp");

  auto main_return_value = llvm::ConstantInt::get(main_return_type, 0);
  ctx.Builder.CreateRet(main_return_value);

  ctx.currentModule->dump();
  std::cout << "Main function verification: " << (!llvm::verifyFunction(*main_func, &llvm::errs()) ? "OK" : "Failed") << std::endl;
}

int main(int argc, char *argv[]) {
  auto file_path = std::string(argv[1]);

  auto module = std::make_unique<llvm::Module>(std::string("example-module"), llvm::getGlobalContext());
  module->setTargetTriple("x86_64-unknown-linux-gnu");
  auto ctx = ir_generator::Context(module);
  generate_example_code(ctx);

  std::error_code ec;
  llvm::raw_fd_ostream raw_output(file_path, ec, llvm::sys::fs::F_Text | llvm::sys::fs::F_Excl);
  ctx.currentModule->print(raw_output, nullptr);
  if(ec) {
    std::cerr << ec.message() << std::endl;
    return EXIT_FAILURE;
  }
  std::cout << "Main module verification: " << (!llvm::verifyModule(*ctx.currentModule, &llvm::errs()) ? "OK" : "Failed") << std::endl;

  ir_compiler::compile(file_path);
}
