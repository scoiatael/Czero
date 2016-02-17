#include "compiler.hpp"

class Compiler {
  ir_generator::Context& ctx;
  llvm::StringMap<llvm::Function*> functions;
  llvm::StringMap<llvm::Value*> variables;
  llvm::Function* current_function;

  llvm::Type* llvm_type_for(ast::types::Type type) {
    switch(type) {
    case ast::types::Bool:
      return llvm::Type::getInt1Ty(this->ctx.llvmContext);
    case ast::types::Float32:
      return llvm::Type::getFloatTy(this->ctx.llvmContext);
    case ast::types::Int32:
      return llvm::Type::getInt32Ty(this->ctx.llvmContext);
    case ast::types::String:
      return llvm::Type::getInt8PtrTy(this->ctx.llvmContext);
    case ast::types::Void:
      return llvm::Type::getVoidTy(this->ctx.llvmContext);
    default:
      assert(false);
    }
    return NULL;
  }

  llvm::Value* compile_bool(ast::types::BoolValue* value) {
    // Because 0 is True in LLVM, and False in C++ (or the other way around..)
    return this->ctx.Builder.getInt1(!value->value);
  }

  llvm::Value* compile_float(ast::types::FloatValue* value) {
    return llvm::ConstantFP::get(llvm_type_for(value->type()), value->value);
  }

  llvm::Value* compile_int(ast::types::IntValue* value) {
    return this->ctx.Builder.getInt32(value->value);
  }

  llvm::Value* compile_string(ast::types::StringValue* value) {
    return this->ctx.Builder.CreateGlobalStringPtr(value->value);
  }

  llvm::Value* value(ast::types::abstract::Value* value) {
    assert(value != nullptr);

    switch(value->type()) {
    case ast::types::Bool:
      return compile_bool(dynamic_cast<ast::types::BoolValue*>(value));
    case ast::types::Float32:
      return compile_float(dynamic_cast<ast::types::FloatValue*>(value));
    case ast::types::Int32:
      return compile_int(dynamic_cast<ast::types::IntValue*>(value));
    case ast::types::String:
      return compile_string(dynamic_cast<ast::types::StringValue*>(value));
    default:
      assert(false);
    }
    return NULL;
  }

  llvm::Value* compile_variable(ast::Variable* var) {
    assert(var != nullptr);

    auto ptr = variables.find(var->identifier);
    assert(ptr != variables.end());
    return this->ctx.Builder.CreateLoad(ptr->getValue());
  }

  llvm::Value* compile_constant(ast::Constant* constant) {
    assert(constant != nullptr);
    return value(constant->value.get());
  }

  llvm::Value* compile_cast(ast::Cast* cast) {
    assert(cast != nullptr);
    auto value = operation(cast->value.get());
    auto type = llvm_type_for(cast->type);
    switch(cast->type) {
    case ast::types::Int32:
      return this->ctx.Builder.CreateIntCast(value,
                                             type,
                                             /* isSigned */ true);
      break;
    case ast::types::Float32:
      return this->ctx.Builder.CreateFPCast(value,
                                            type);
      break;
    case ast::types::Bool:
      return this->ctx.Builder.CreateIntCast(value,
                                             type,
                                             /* isSigned */ false);
      break;
    case ast::types::Void:
      // No-op for sake of checker
      return value;
    default:
      assert(false);
    }
    assert(false);
    return NULL;
  }

  llvm::Value* compile_un_op(ast::UnOp* un_op) {
    assert(un_op != nullptr);
    auto value = operation(un_op->value.get());
    switch(un_op->type) {
    case ast::types::Int32:
      if(un_op->operator_ == "neg") {
        return this->ctx.Builder.CreateNeg(value);
      }
      break;
    case ast::types::Float32:
      if(un_op->operator_ == "neg") {
        return this->ctx.Builder.CreateFNeg(value);
      }
      break;
    case ast::types::Bool:
      if(un_op->operator_ == "not") {
        return this->ctx.Builder.CreateNot(value);
      }
      break;
    default:
      assert(false);
    }
    assert(false);
    return NULL;
  }

  llvm::Value* compile_bin_op(ast::BinOp* bin_op) {
    assert(bin_op != nullptr);
    auto lhs_value = operation(bin_op->left.get());
    auto rhs_value = operation(bin_op->right.get());
    switch(bin_op->type) {
    case ast::types::Int32:
      if(bin_op->operator_ == "add") {
        return this->ctx.Builder.CreateAdd(lhs_value, rhs_value);
      }
      if(bin_op->operator_ == "sub") {
        return this->ctx.Builder.CreateSub(lhs_value, rhs_value);
      }
      if(bin_op->operator_ == "mul") {
        return this->ctx.Builder.CreateMul(lhs_value, rhs_value);
      }
      if(bin_op->operator_ == "div") {
        return this->ctx.Builder.CreateSDiv(lhs_value, rhs_value);
      }
      break;
    case ast::types::Float32:
      if(bin_op->operator_ == "add") {
        return this->ctx.Builder.CreateFAdd(lhs_value, rhs_value);
      }
      if(bin_op->operator_ == "sub") {
        return this->ctx.Builder.CreateFSub(lhs_value, rhs_value);
      }
      if(bin_op->operator_ == "mul") {
        return this->ctx.Builder.CreateFMul(lhs_value, rhs_value);
      }
      if(bin_op->operator_ == "div") {
        return this->ctx.Builder.CreateFDiv(lhs_value, rhs_value);
      }
      break;
    case ast::types::Bool:
      if(bin_op->operator_ == "and") {
        return this->ctx.Builder.CreateAnd(lhs_value, rhs_value);
      }
      if(bin_op->operator_ == "or") {
        return this->ctx.Builder.CreateOr(lhs_value, rhs_value);
      }
      switch (bin_op->from) {
      case ast::types::Float32:
        if(bin_op->operator_ == "eq") {
          return this->ctx.Builder.CreateFCmpOEQ(lhs_value, rhs_value);
        }
        if(bin_op->operator_ == "lt") {
          return this->ctx.Builder.CreateFCmpOLT(lhs_value, rhs_value);
        }
        break;
      case ast::types::Int32:
        if(bin_op->operator_ == "eq") {
          return this->ctx.Builder.CreateICmpEQ(lhs_value, rhs_value);
        }
        if(bin_op->operator_ == "lt") {
          return this->ctx.Builder.CreateICmpSLT(lhs_value, rhs_value);
        }
        break;
      case ast::types::Bool:
        if(bin_op->operator_ == "eq") {
          return this->ctx.Builder.CreateICmpEQ(lhs_value, rhs_value);
        }
        if(bin_op->operator_ == "lt") {
          return this->ctx.Builder.CreateICmpULT(lhs_value, rhs_value);
        }
        break;
      default:
        assert(false);
      }
      break;
    default:
      assert(false);
    }
    assert(false);
    return NULL;
  }

  llvm::Value* compile_call(ast::Call* call) {
    assert(call != nullptr);

    auto func = functions.find(call->function_name);
    assert(func != functions.end());
    std::vector<llvm::Value*> arguments;
    for (auto it = call->arguments.begin(); it != call->arguments.end(); ++it) {
      arguments.push_back(operation(it->get()));
    }
    return this->ctx.Builder.CreateCall(func->getValue(), arguments);
  }

  int compile_return(ast::Return* retrn) {
    assert(retrn != nullptr);
    assert(retrn->value.get() != nullptr);
    if(retrn->value->type == ast::types::Void) {
      this->ctx.Builder.CreateRetVoid();
    } else {
      auto val = operation(retrn->value.get());
      this->ctx.Builder.CreateRet(val);
    }
    return EXIT_SUCCESS;
  }

  int compile_branch(ast::Branch* branch) {
    assert(branch != nullptr);
    assert(this->current_function != nullptr);
    auto condition_block = llvm::BasicBlock::Create(this->ctx.llvmContext,
                                                    "condition",
                                                    this->current_function);
    auto then_block = llvm::BasicBlock::Create(this->ctx.llvmContext,
                                               "then",
                                               this->current_function);
    auto else_block = llvm::BasicBlock::Create(this->ctx.llvmContext,
                                               "else",
                                               this->current_function);
    auto exit_block = llvm::BasicBlock::Create(this->ctx.llvmContext,
                                               "exit",
                                               this->current_function);
    this->ctx.Builder.CreateBr(condition_block);
    this->ctx.Builder.SetInsertPoint(condition_block);
    auto jump_condition = operation(branch->condition.get());
    this->ctx.Builder.CreateCondBr(jump_condition, then_block, else_block);
    this->ctx.Builder.SetInsertPoint(then_block);
    compile_body(branch->then_);
    this->ctx.Builder.CreateBr(exit_block);
    this->ctx.Builder.SetInsertPoint(else_block);
    compile_body(branch->else_);
    this->ctx.Builder.CreateBr(exit_block);
    this->ctx.Builder.SetInsertPoint(exit_block);
    return EXIT_SUCCESS;
  }

  int compile_assignment(ast::Assignment* assignment) {
    assert(assignment != nullptr);
    auto ptr = variables.find(assignment->identifier);
    assert(ptr != variables.end());

    auto val = operation(assignment->value.get());
    this->ctx.Builder.CreateStore(val, ptr->getValue());
    return EXIT_SUCCESS;
  }

  int compile_while(ast::While* whil) {
    assert(whil != nullptr);
    assert(this->current_function != nullptr);
    auto condition_block = llvm::BasicBlock::Create(this->ctx.llvmContext,
                                                    "condition",
                                                    this->current_function);
    auto body_block = llvm::BasicBlock::Create(this->ctx.llvmContext,
                                               "body",
                                               this->current_function);
    auto exit_block = llvm::BasicBlock::Create(this->ctx.llvmContext,
                                               "exit",
                                               this->current_function);
    this->ctx.Builder.CreateBr(condition_block);
    this->ctx.Builder.SetInsertPoint(condition_block);
    auto jump_condition = operation(whil->condition.get());
    this->ctx.Builder.CreateCondBr(jump_condition, body_block, exit_block);
    this->ctx.Builder.SetInsertPoint(body_block);
    compile_body(whil->body);
    this->ctx.Builder.CreateBr(condition_block);
    this->ctx.Builder.SetInsertPoint(exit_block);
    return EXIT_SUCCESS;
  }

  int compile_void_context(ast::VoidContext* void_context) {
    assert(void_context != nullptr);
    operation(void_context->operation.get());
    return EXIT_SUCCESS;
  }

  int compile_body(ast::Body body) {
    for (auto it = body.begin(); it != body.end(); ++it) {
      statement(it->get());
    }
    return EXIT_SUCCESS;
  }

  llvm::Function* compile_extern(ast::Extern* extrn) {
    auto argst = std::vector<llvm::Type*>();
    for (auto it = extrn->arguments.begin(); it != extrn->arguments.end(); ++it) {
      argst.push_back(llvm_type_for(it->type));
    }

    auto extrnt =
      llvm::FunctionType::get(llvm_type_for(extrn->return_value),
                              argst,
                              extrn->variadic);

    auto extrnf =
      llvm::Function::Create(extrnt,
                             llvm::Function::ExternalLinkage,
                             extrn->identifier,
                             this->ctx.currentModule.get());

    assert(functions.find(extrn->identifier) == functions.end());
    functions.insert(std::pair<llvm::StringRef, llvm::Function*>(extrn->identifier,
                                                                 extrnf));
    return extrnf;
  }

  int allocate_variable(ast::Variable& it, llvm::Value* value = nullptr) {
    auto alloc_inst = this->ctx.Builder.CreateAlloca(llvm_type_for(it.type));
    variables.insert(std::pair<llvm::StringRef, llvm::Value*>(it.identifier,
                                                              alloc_inst));
    if(value != nullptr) {
      this->ctx.Builder.CreateStore(value, alloc_inst);
    }
    return EXIT_SUCCESS;
  }

  int compile_func(ast::Func* func) {
    assert(func != nullptr);
    auto decl = compile_extern(dynamic_cast<ast::Extern*>(func));

    auto block = llvm::BasicBlock::Create(this->ctx.llvmContext, "entry", decl);
    this->ctx.Builder.SetInsertPoint(block);

    // Allocate memory on stack for function argument variables
    for (auto it = func->arguments.begin(); it != func->arguments.end(); ++it) {
      auto index = std::distance(func->arguments.begin(), it);
      auto arg = std::next(decl->arg_begin(), index);
      assert(arg != decl->arg_end());
      allocate_variable(*it, arg);
    }
    // Allocate memory on stack for local variables
    for (auto it = func->local_variables.begin(); it != func->local_variables.end(); ++it) {
      allocate_variable(*it);
    }
    this->current_function = decl;
    compile_body(func->body);

    variables.clear();
    return EXIT_SUCCESS;
  }

  llvm::Value* operation(ast::abstract::Operation* op) {
    assert(op != nullptr);
    switch(*op) {
    case ast::VariableNode:
      return compile_variable(dynamic_cast<ast::Variable*>(op));
      break;
    case ast::ConstantNode:
      return compile_constant(dynamic_cast<ast::Constant*>(op));
      break;
    case ast::CastNode:
      return compile_cast(dynamic_cast<ast::Cast*>(op));
      break;
    case ast::UnOpNode:
      return compile_un_op(dynamic_cast<ast::UnOp*>(op));
      break;
    case ast::BinOpNode:
      return compile_bin_op(dynamic_cast<ast::BinOp*>(op));
      break;
    case ast::CallNode:
      return compile_call(dynamic_cast<ast::Call*>(op));
      break;
    default:
      assert(false);
    }
    return NULL;
  }

  int statement(ast::abstract::Statement* statement) {
    assert(statement != nullptr);
    switch(*statement) {
    case ast::ReturnNode:
      compile_return(dynamic_cast<ast::Return*>(statement));
      break;
    case ast::BranchNode:
      compile_branch(dynamic_cast<ast::Branch*>(statement));
      break;
    case ast::AssignmentNode:
      compile_assignment(dynamic_cast<ast::Assignment*>(statement));
      break;
    case ast::WhileNode:
      compile_while(dynamic_cast<ast::While*>(statement));
      break;
    case ast::VoidContextNode:
      compile_void_context(dynamic_cast<ast::VoidContext*>(statement));
      break;
    default:
      assert(false);
    }
    return EXIT_SUCCESS;
  }

  int node(ast::abstract::Node* node) {
    assert(node != nullptr);
    switch(*node) {
    case ast::ExternNode:
      compile_extern(dynamic_cast<ast::Extern*>(node));
      break;
    case ast::FuncNode:
      compile_func(dynamic_cast<ast::Func*>(node));
      break;
    default:
      assert(false);
    }
    return EXIT_SUCCESS;
  }

public:
  Compiler(ir_generator::Context& context)
    : ctx(context) { }

  int program(ast::Program* program) {
    assert(program != nullptr);
    for (auto it = program->body.begin(); it != program->body.end(); ++it) {
      node(it->get());
    }
    // this->ctx.currentModule->dump();
    return EXIT_SUCCESS;
  }
};

int compiler::program(ast::Program* program, ir_generator::Context& ctx) {
  return Compiler(ctx).program(program);
}
