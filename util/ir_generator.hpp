#ifndef IR_GENERATOR_INCLUDED
#define IR_GENERATOR_INCLUDED    1
#include <iostream>

#include <llvm/ADT/STLExtras.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>

#include <llvm/Support/FileSystem.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/raw_os_ostream.h>

#include <memory>

namespace ir_generator {
  struct Options {
    bool verbose = false;
    bool verify = true;
    bool force_overwrite = false;
    llvm::raw_ostream& err = llvm::errs();
    llvm::raw_ostream& log = llvm::outs();
  };

  struct Context {
    std::unique_ptr<llvm::Module> currentModule;
    llvm::LLVMContext &llvmContext = llvm::getGlobalContext();
    llvm::IRBuilder<> Builder = llvm::IRBuilder<>(llvmContext);

    Context(std::string name);

  };

  int print_module(std::string file_path, Context& ctx, const Options& options = Options());
  int generate_example_code(Context& ctx, const Options& options = Options());
}

#endif
