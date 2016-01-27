#include "ir_generator.hpp"

ir_generator::Context::Context(std::string name) {
  auto module = std::make_unique<llvm::Module>(name, llvm::getGlobalContext());
  module->setTargetTriple("x86_64-unknown-linux-gnu");
  this->currentModule = std::move(module);
}

int ir_generator::generate_example_code(ir_generator::Context& ctx, const Options& options) {
  auto int8t = llvm::Type::getInt8Ty(ctx.llvmContext);
  auto int8ptrt = llvm::PointerType::getUnqual(int8t);
  auto printf_argst = std::vector<llvm::Type*>(1, int8ptrt);
  auto int32t = llvm::Type::getInt32Ty(ctx.llvmContext);

  auto printf_type =
    llvm::FunctionType::get(int32t, printf_argst, true);

  auto printf_func =
    llvm::Function::Create(printf_type, llvm::Function::ExternalLinkage, "printf", ctx.currentModule.get());

  auto main_returnt = int32t;
  auto main_funct =
    llvm::FunctionType::get(main_returnt, false);

  auto main_func =
    llvm::Function::Create(main_funct, llvm::Function::ExternalLinkage, "main", ctx.currentModule.get());

  auto block = llvm::BasicBlock::Create(ctx.llvmContext, "entry", main_func);
  ctx.Builder.SetInsertPoint(block);

  auto helloWorld = ctx.Builder.CreateGlobalStringPtr("hello world!\n");
  auto printf_args = std::vector< llvm::Value* >(1, helloWorld);

  ctx.Builder.CreateCall(printf_func, printf_args, "calltmp");

  auto main_return_value = llvm::ConstantInt::get(main_returnt, 0);
  ctx.Builder.CreateRet(main_return_value);

  if (options.verbose) {
    ctx.currentModule->dump();
  }
  auto main_ok = true;
  if (options.verify) {
    main_ok = !llvm::verifyFunction(*main_func, &options.err);
    if (options.verbose || !main_ok) {
      options.log << "Main function verification: " << (main_ok ? "OK" : "Failed") << "\n";
    }
  }
  return main_ok ? EXIT_SUCCESS : EXIT_FAILURE;
}

int ir_generator::print_module(std::string file_path, ir_generator::Context& ctx, const Options& options) {
  std::error_code ec;
  auto flags = llvm::sys::fs::F_Text;
  if (!options.force_overwrite) {
    flags = flags | llvm::sys::fs::F_Excl;
  }
  llvm::raw_fd_ostream raw_output(file_path, ec, flags);
  ctx.currentModule->print(raw_output, nullptr);
  if(ec) {
    options.err << "Failed printing module: " << ec.message() << "\n";
    return EXIT_FAILURE;
  }

  auto main_ok = true;
  if(options.verify) {
    main_ok = !llvm::verifyModule(*ctx.currentModule, &options.err);
    if (options.verbose || !main_ok) {
      options.log << "Main module verification: " << (main_ok ? "OK" : "Failed") << "\n";
    }
  }
  return main_ok ? EXIT_SUCCESS : EXIT_FAILURE;
}
