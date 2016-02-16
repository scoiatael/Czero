#include "checker.hpp"

class Checker {
  llvm::StringMap<ast::Extern> functions;
  llvm::StringMap<ast::Variable> variables;
  ast::Extern* current_function;

  void type_mismatch(ast::types::Type ltype, ast::types::Type rtype) {
    std::cout << "\t(expected ";
    pretty_print::type(std::cout, ltype);
    std::cout << ", got ";
    pretty_print::type(std::cout, rtype);
    std::cout << ")\n";
  }

  void print_id(int id) {
    if(id > -1) {
      std::cout << " at "
                << id;
    }
  }

  int assert_type(ast::types::Type ltype, ast::types::Type rtype, std::string operation, int id = -1) {
    if(ltype != rtype) {
      std::cout << "Bad type of "
                << operation;
      print_id(id);
      std::cout << std::endl;
      type_mismatch(ltype, rtype);
      return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
  }

  int check_coercion(ast::types::Type& ltype, ast::types::Type rtype, std::string operation, int id = -1) {
    if(ltype == ast::types::Poison) {
      ltype = rtype;
    }
    return assert_type(ltype, rtype, operation, id);
  }

  void assert_existing_variable(llvm::StringMapIterator<ast::Variable> ptr, std::string identifier, int id = -1) {
    if(ptr == variables.end()) {
      std::cout << "Unknown variable "
                << identifier;
      print_id(id);
      std::cout << std::endl;
    }
  }

  int check_variable(ast::Variable* var) {
    assert(var != nullptr);

    auto ptr = variables.find(var->identifier);
    assert_existing_variable(ptr, var->identifier, var->id);
    auto expected = ptr->getValue().type;
    return check_coercion(var->type, expected, var->identifier, var->id);
  }

  int check_constant(ast::Constant* constant) {
    assert(constant != nullptr);
    auto expected = constant->value->type();
    return check_coercion(constant->type, expected, "constant", constant->id);
  }

  int check_cast(ast::Cast* cast) {
    assert(cast != nullptr);
    auto value = cast->value.get();
    check_operation(value);
    switch(cast->type) {
    case ast::types::Int32:
    case ast::types::Float32:
    case ast::types::Bool:
    case ast::types::Void:
      break;
    default:
      std::cout << "Bad cast type: "
                << cast->type
                << " at "
                << cast->id
                << std::endl;
      return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
  }

  int check_un_op(ast::UnOp* un_op) {
    assert(un_op != nullptr);
    auto value = un_op->value.get();
    check_operation(value);
    switch(un_op->type) {
    case ast::types::Int32:
      if(un_op->operator_ == "neg") {
        return assert_type(un_op->type, value->type, un_op->operator_, un_op->id);
      }
      break;
    case ast::types::Float32:
      if(un_op->operator_ == "neg") {
        return assert_type(un_op->type, value->type, un_op->operator_, un_op->id);
      }
      break;
    case ast::types::Bool:
      if(un_op->operator_ == "not") {
        return assert_type(un_op->type, value->type, un_op->operator_, un_op->id);
      }
      break;
    default:
      break;
    }
    std::cout << "Unknown unary operation "
              << un_op->operator_
              << " of type "
              << value->type
              << " -> "
              << un_op->type
              << " at "
              << un_op->id
              << std::endl;
    return EXIT_FAILURE;
  }

  int check_bin_op(ast::BinOp* bin_op) {
    assert(bin_op != nullptr);
    auto lhs = bin_op->left.get();
    auto rhs = bin_op->right.get();
    check_operation(lhs);
    check_operation(rhs);
    check_coercion(bin_op->from, lhs->type, bin_op->operator_, bin_op->id);
    check_coercion(bin_op->from, rhs->type, bin_op->operator_, bin_op->id);
    auto operators_without_coercion =
      std::set<std::string>{"add", "sub", "mul", "div", "and", "or"};
    if(operators_without_coercion.count(bin_op->operator_) > 0) {
      return check_coercion(bin_op->type, bin_op->from, bin_op->operator_, bin_op->id);
    }
    auto operators_with_coercion_to_bool =
      std::set<std::string>{"eq", "lt"};
    if(operators_with_coercion_to_bool.count(bin_op->operator_) > 0) {
      return check_coercion(bin_op->type, ast::types::Bool, bin_op->operator_, bin_op->id);
    }
    std::cout << "Unknown binary operation "
              << bin_op->operator_
              << " of type "
              << lhs->type
              << " * "
              << rhs->type
              << " -> "
              << bin_op->type
              << " at "
              << bin_op->id
              << std::endl;
    return EXIT_FAILURE;
  }

  int check_call(ast::Call* call) {
    assert(call != nullptr);

    auto func = functions.find(call->function_name);
    if(func == functions.end()) {
      std::cout << "Unknown function "
                << call->function_name
                << " at "
                << call->id
                << std::endl;
      return EXIT_FAILURE;
    }
    auto fun = func->getValue();
    auto actual_argument_size = call->arguments.size();
    auto prototype_argument_size = fun.arguments.size();
    if(actual_argument_size < prototype_argument_size) {
      std::cout << "Not enough arguments to "
                << fun.identifier
                << " (expected "
                << prototype_argument_size
                << ", got "
                << actual_argument_size
                << ")"
                << std::endl;
      return EXIT_FAILURE;
    }
    if(actual_argument_size > prototype_argument_size && !fun.variadic) {
      std::cout << "Too many arguments to "
                << fun.identifier
                << " (expected "
                << prototype_argument_size
                << ", got "
                << actual_argument_size
                << ")"
                << std::endl;
      return EXIT_FAILURE;
    }
    check_coercion(call->type, fun.return_value, fun.identifier, call->id);
    for (auto it = call->arguments.begin(); it != call->arguments.end(); ++it) {
      auto op = it->get();
      auto index = std::distance(call->arguments.begin(), it);
      auto prototype_op = std::next(fun.arguments.begin(), index);
      check_operation(op);
      // Variadic functions.. don't check type.
      if(prototype_op != fun.arguments.end()) {
        check_coercion(op->type, prototype_op->type, "function argument", op->id);
      }
    }
    return EXIT_SUCCESS;
  }

  int check_return(ast::Return* retrn) {
    assert(retrn != nullptr);
    assert(this->current_function != nullptr);
    auto op = retrn->value.get();
    check_operation(op);
    return check_coercion(op->type, this->current_function->return_value, "return", retrn->id);
  }

  int check_branch(ast::Branch* branch) {
    assert(branch != nullptr);
    auto cond = branch->condition.get();
    check_operation(cond);
    check_coercion(cond->type, ast::types::Bool, "condition", branch->id);
    check_body(branch->then_);
    check_body(branch->else_);
    return EXIT_SUCCESS;
  }

  int check_assignment(ast::Assignment* assignment) {
    assert(assignment != nullptr);
    auto ptr = variables.find(assignment->identifier);
    assert_existing_variable(ptr, assignment->identifier, assignment->id);
    auto val = assignment->value.get();
    check_operation(val);
    return check_coercion(val->type, ptr->getValue().type, "assignment", assignment->id);
  }

  int check_while(ast::While* whil) {
    assert(whil != nullptr);
    auto cond = whil->condition.get();
    check_operation(cond);
    check_coercion(cond->type, ast::types::Bool, "condition", whil->id);
    check_body(whil->body);
    return EXIT_SUCCESS;
  }

  int check_void_context(ast::VoidContext* void_context) {
    assert(void_context != nullptr);
    auto op = void_context->operation.get();
    check_operation(op);
    return check_coercion(op->type, ast::types::Void, "void context", void_context->id);
  }

  int check_body(ast::Body body) {
    for (auto it = body.begin(); it != body.end(); ++it) {
      check_statement(it->get());
    }
    return EXIT_SUCCESS;
  }

  int declare_extern(ast::Extern* extrn) {
    assert(extrn != nullptr);
    if(functions.find(extrn->identifier) != functions.end()) {
      std::cout << "Double declaration of "
                << extrn->identifier;
      print_id(extrn->id);
      std::cout << std::endl;
    }
    functions.insert(std::pair<llvm::StringRef, ast::Extern>(extrn->identifier,
                                                                 *extrn));
    return EXIT_SUCCESS;
  }

  int declare_variable(ast::Variable& it) {
    if(variables.find(it.identifier) != variables.end()) {
      std::cout << "Variable shadowing of "
                << it.identifier;
      print_id(it.id);
      std::cout << std::endl;
    }
    variables.insert(std::pair<llvm::StringRef, ast::Variable>(it.identifier,
                                                               it));
    return EXIT_SUCCESS;
  }

  int check_func(ast::Func* func) {
    assert(func != nullptr);

    for (auto it = func->arguments.begin(); it != func->arguments.end(); ++it) {
      declare_variable(*it);
    }
    // Allocate memory on stack for local variables
    for (auto it = func->local_variables.begin(); it != func->local_variables.end(); ++it) {
      declare_variable(*it);
    }
    this->current_function = func;
    check_body(func->body);

    // TODO: check for return value?
    this->current_function = nullptr;
    variables.clear();
    return EXIT_SUCCESS;
  }

  int check_operation(ast::abstract::Operation* op) {
    assert(op != nullptr);
    switch(*op) {
    case ast::VariableNode:
      return check_variable(dynamic_cast<ast::Variable*>(op));
      break;
    case ast::ConstantNode:
      return check_constant(dynamic_cast<ast::Constant*>(op));
      break;
    case ast::CastNode:
      return check_cast(dynamic_cast<ast::Cast*>(op));
      break;
    case ast::UnOpNode:
      return check_un_op(dynamic_cast<ast::UnOp*>(op));
      break;
    case ast::BinOpNode:
      return check_bin_op(dynamic_cast<ast::BinOp*>(op));
      break;
    case ast::CallNode:
      return check_call(dynamic_cast<ast::Call*>(op));
      break;
    }
    assert(false);
    return EXIT_FAILURE;
  }

  int check_statement(ast::abstract::Statement* statement) {
    assert(statement != nullptr);
    switch(*statement) {
    case ast::ReturnNode:
      return check_return(dynamic_cast<ast::Return*>(statement));
      break;
    case ast::BranchNode:
      return check_branch(dynamic_cast<ast::Branch*>(statement));
      break;
    case ast::AssignmentNode:
      return check_assignment(dynamic_cast<ast::Assignment*>(statement));
      break;
    case ast::WhileNode:
      return check_while(dynamic_cast<ast::While*>(statement));
      break;
    case ast::VoidContextNode:
      return check_void_context(dynamic_cast<ast::VoidContext*>(statement));
      break;
    }
    assert(false);
    return EXIT_SUCCESS;
  }

  int check_node(ast::abstract::Node* node) {
    assert(node != nullptr);
    switch(*node) {
    case ast::ExternNode:
      return EXIT_SUCCESS;
    case ast::FuncNode:
      return check_func(dynamic_cast<ast::Func*>(node));
      break;
    }
    assert(false);
    return EXIT_SUCCESS;
  }

  int declare_node(ast::abstract::Node* node) {
    assert(node != nullptr);
    switch(*node) {
    case ast::ExternNode:
    case ast::FuncNode:
      return declare_extern(dynamic_cast<ast::Extern*>(node));
      break;
    }
    assert(false);
    return EXIT_SUCCESS;
  }

public:
  int program(ast::Program* program) {
    assert(program != nullptr);
    for (auto it = program->body.begin(); it != program->body.end(); ++it) {
      declare_node(it->get());
    }

    for (auto it = program->body.begin(); it != program->body.end(); ++it) {
      check_node(it->get());
    }
    return EXIT_SUCCESS;
  }
};

int checker::program(ast::Program* program) {
  return Checker().program(program);
}
