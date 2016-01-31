#include "compiler.hpp"

class Compiler {
  ir_generator::Context& ctx;
  llvm::StringMap<llvm::Function*> functions;
  llvm::StringMap<llvm::Value*> variables;

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
    default:
      assert(false);
    }
    return NULL;
  }

  llvm::Value* compile_bool(ast::types::BoolValue* value) {
    return this->ctx.Builder.getInt1(value->value);
  }

  llvm::Value* compile_float(ast::types::FloatValue* value) {
    return llvm::ConstantFP::get(llvm_type_for(value->type()), value->value);
  }

  llvm::Value* compile_int(ast::types::IntValue* value) {
    return this->ctx.Builder.getInt32(value->value);
  }

  llvm::Value* compile_string(ast::types::StringValue* value) {
    return ctx.Builder.CreateGlobalStringPtr(value->value);
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
    auto ptr = variables.find(var->identifier);
    assert(ptr != variables.end());
    return this->ctx.Builder.CreateLoad(ptr->getValue());
  }

  llvm::Value* compile_constant(ast::Constant* constant) {
    assert(constant != nullptr);
    return value(constant->value.get());
  }

  llvm::Value* compile_cast(ast::Cast* cast) {
    return NULL;
  }

  llvm::Value* compile_un_op(ast::UnOp* un_op) {
    return NULL;
  }

  llvm::Value* compile_bin_op(ast::BinOp* bin_op) {
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
    auto val = operation(retrn->value.get());
    this->ctx.Builder.CreateRet(val);
    return EXIT_SUCCESS;
  }

  int compile_branch(ast::Branch* statement) {
    return EXIT_SUCCESS;
  }

  int compile_assignment(ast::Assignment* assignment) {
    assert(assignment != nullptr);
    auto ptr = variables.find(assignment->identifier);
    assert(ptr != variables.end());

    auto val = operation(assignment->value.get());
    this->ctx.Builder.CreateStore(ptr->getValue(), val);
    return EXIT_SUCCESS;
  }

  int compile_while(ast::While* statement) {
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

  int compile_func(ast::Func* func) {
    assert(func != nullptr);
    auto decl = compile_extern(dynamic_cast<ast::Extern*>(func));

    auto block = llvm::BasicBlock::Create(this->ctx.llvmContext, "entry", decl);
    this->ctx.Builder.SetInsertPoint(block);

    // Allocate memory on stack for function argument variables
    for (auto it = func->arguments.begin(); it != func->arguments.end(); ++it) {
      auto alloc_inst = this->ctx.Builder.CreateAlloca(llvm_type_for(it->type));
      variables.insert(std::pair<llvm::StringRef, llvm::Value*>(it->identifier,
                                                                alloc_inst));
    }
    // Allocate memory on stack for local variables
    for (auto it = func->local_variables.begin(); it != func->local_variables.end(); ++it) {
      auto alloc_inst = this->ctx.Builder.CreateAlloca(llvm_type_for(it->type));
      variables.insert(std::pair<llvm::StringRef, llvm::Value*>(it->identifier,
                                                                alloc_inst));
    }
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
    this->ctx.currentModule->dump();
    return EXIT_SUCCESS;
  }
};

int compiler::program(ast::Program* program, ir_generator::Context& ctx) {
  return Compiler(ctx).program(program);
}
