#include "pretty_print.hpp"

class PrettyPrinter {
  int indent = 0;

  void print_indent() {
    for (int i = 0; i < this->indent; i++) {
      std::cout << " ";
    }
  }

  int print_func_header(ast::Extern* extrn) {
    assert(extrn != nullptr);
    std::cout << extrn->identifier << "(";
    for (auto it = extrn->arguments.begin(); it != extrn->arguments.end(); ++it) {
      std::cout << it->identifier << " : ";
      type(it->type);
      if((it + 1) != extrn->arguments.end()) {
        std::cout << ", ";
      }
    }
    std::cout << (extrn->variadic ? ", ..." : "") << ")" << " : ";
    type(extrn->return_value);
    return EXIT_SUCCESS;
  }

  int print_func(ast::Func* func) {
    assert(func != nullptr);
    std::cout << "fun ";
    print_func_header(dynamic_cast<ast::Extern*>(func));
    std::cout << " {\n";
    this->indent = 2;
    print_indent();
    std::cout <<  "vars {\n";
    this->indent += 2;
    for (auto it = func->local_variables.begin(); it != func->local_variables.end(); ++it) {
      print_indent();
      type(it->type);
      std::cout << " " << it->identifier << ";\n";
    }
    this->indent -= 2;
    print_indent();
    std::cout << "}\n";
    print_body(func->body);
    std::cout << "}\n";
    this->indent = 0;
    return EXIT_SUCCESS;
  }

  int print_extern(ast::Extern* extrn) {
    assert(extrn != nullptr);
    std::cout << "extern ";
    print_func_header(extrn);
    std::cout << ";\n";
    return EXIT_SUCCESS;
  }

  int print_variable(ast::Variable* variable) {
    assert(variable != nullptr);
    type(variable->type);
    std::cout << " " << variable->identifier;
    return EXIT_SUCCESS;
  }

  int print_constant(ast::Constant* constant) {
    assert(constant != nullptr);
    value(dynamic_cast<ast::types::abstract::Value*>(constant->value.get()));
    return EXIT_SUCCESS;
  }

  int print_cast(ast::Cast* cast) {
    assert(cast != nullptr);
    std::cout << "cast";
    type(cast->to);
    std::cout << " ";
    node(dynamic_cast<ast::abstract::Node*>(cast->value.get()));
    return EXIT_SUCCESS;
  }

  int print_un_op(ast::UnOp* un_op) {
    assert(un_op != nullptr);
    std::cout << un_op->operator_;
    std::cout << " ";
    node(dynamic_cast<ast::abstract::Node*>(un_op->value.get()));
    return EXIT_SUCCESS;
  }

  int print_bin_op(ast::BinOp* bin_op) {
    assert(bin_op != nullptr);
    std::cout << "(";
    node(dynamic_cast<ast::abstract::Node*>(bin_op->left.get()));
    std::cout << ")" << bin_op->operator_ << "(";
    node(dynamic_cast<ast::abstract::Node*>(bin_op->right.get()));
    std::cout << ")";
    return EXIT_SUCCESS;
  }

  int print_call(ast::Call* call) {
    assert(call != nullptr);
    std::cout << call->function_name << "(";
    for (auto it = call->arguments.begin(); it != call->arguments.end(); ++it) {
      node(it->get());
      if((it + 1) != call->arguments.end()) {
        std::cout << ", ";
      }
    }
    std::cout << ")";
    return EXIT_SUCCESS;
  }

  int print_return(ast::Return* retrn) {
    print_indent();
    assert(retrn != nullptr);
    std::cout << "return ";
    node(dynamic_cast<ast::abstract::Node*>(retrn->value.get()));
    std::cout << ";\n";
    return EXIT_SUCCESS;
  }

  int print_branch(ast::Branch* branch) {
    assert(branch != nullptr);
    print_indent();
    std::cout << "if(";
    node(dynamic_cast<ast::abstract::Node*>(branch->condition.get()));
    std::cout << ") {\n";
    this->indent += 2;
    print_body(branch->then_);
    this->indent -= 2;
    std::cout << " }\n else {\n";
    this->indent += 2;
    print_body(branch->else_);
    this->indent -= 2;
    std::cout << "}\n";
    return EXIT_SUCCESS;
  }

  int print_assignment(ast::Assignment* assignment) {
    assert(assignment != nullptr);
    print_indent();
    std::cout << assignment->identifier << " := ";
    node(assignment->value.get());
    std::cout << ";\n";
    return EXIT_SUCCESS;
  }

  int print_body(ast::Body body) {
    for (auto it = body.begin(); it != body.end(); ++it) {
      node(it->get());
    }
    return EXIT_SUCCESS;
  }

  int print_while(ast::While* whil) {
    assert(whil != nullptr);
    print_indent();
    std::cout << "while(";
    node(whil->condition.get());
    std::cout << ") {\n";
    this->indent += 2;
    print_body(whil->body);
    this->indent -= 2;
    std::cout << "}\n";
    return EXIT_SUCCESS;
  }

  int print_void_context(ast::VoidContext* void_context) {
    assert(void_context != nullptr);
    print_indent();
    node(void_context->operation.get());
    std::cout << ";\n";
    return EXIT_SUCCESS;
  }

  ast::types::BoolValue* asBool(ast::types::abstract::Value* value) {
    auto ptr =  dynamic_cast<ast::types::BoolValue*>(value);
    assert(ptr != nullptr);
    return ptr;
  }

  ast::types::FloatValue* asFloat(ast::types::abstract::Value* value) {
    auto ptr = dynamic_cast<ast::types::FloatValue*>(value);
    assert(ptr != nullptr);
    return ptr;
  }

  ast::types::IntValue* asInt(ast::types::abstract::Value* value) {
    auto ptr = dynamic_cast<ast::types::IntValue*>(value);
    assert(ptr != nullptr);
    return ptr;
  }

  ast::types::StringValue* asString(ast::types::abstract::Value* value) {
    auto ptr = dynamic_cast<ast::types::StringValue*>(value);
    assert(ptr != nullptr);
    return ptr;
  }

public:

  int node_type(ast::RealNodeType node_type) {
    switch(node_type) {
    case ast::VariableNode:
      std::cout << "[VariableNode]";
      break;
    case ast::ExternNode:
      std::cout << "[ExternNode]";
      break;
    case ast::FuncNode:
      std::cout << "[FuncNode]";
      break;
    case ast::ConstantNode:
      std::cout << "[ConstantNode]";
      break;
    case ast::CastNode:
      std::cout << "[CastNode]";
      break;
    case ast::UnOpNode:
      std::cout << "[UnOpNode]";
      break;
    case ast::BinOpNode:
      std::cout << "[BinOpNode]";
      break;
    case ast::CallNode:
      std::cout << "[CallNode]";
      break;
    case ast::ReturnNode:
      std::cout << "[ReturnNode]";
      break;
    case ast::BranchNode:
      std::cout << "[BranchNode]";
      break;
    case ast::AssignmentNode:
      std::cout << "[AssignmentNode]";
      break;
    case ast::WhileNode:
      std::cout << "[WhileNode]";
      break;
    case ast::VoidContextNode:
      std::cout << "[VoidContextNode]";
      break;
    default:
      std::cout << "[Unknown Value]";
      return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
  }

  int type(ast::types::Type type) {
    switch(type) {
    case ast::types::Bool:
      std::cout << "[Bool]";
      break;
    case ast::types::Float32:
      std::cout << "[Float32]";
      break;
    case ast::types::Int32:
      std::cout << "[Int32]";
      break;
    case ast::types::String:
      std::cout << "[String]";
      break;
    default:
      std::cout << "[Unknown Value]";
      return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
  }

  int value(ast::types::abstract::Value* value) {
    const boost::regex esc("\n");
    const std::string rep("\\\\n");

    assert(value != nullptr);
    type(value->type());
    std::cout << " ";
    switch(*value) {
    case ast::types::Bool:
      std::cout << asBool(value)->value;
      break;
    case ast::types::Float32:
      std::cout << asFloat(value)->value;
      break;
    case ast::types::Int32:
      std::cout << asInt(value)->value;
      break;
    case ast::types::String:
      std::cout << "\""
                << boost::regex_replace(asString(value)->value, esc, rep,
                                        boost::match_default | boost::format_sed)
                << "\"";
      break;
    default:
      std::cout << " ??";
      return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
  }

  int node(ast::abstract::Node* node) {
    assert(node != nullptr);
    switch(*node) {
    case ast::ExternNode:
      print_extern(dynamic_cast<ast::Extern*>(node));
      break;
    case ast::FuncNode:
      print_func(dynamic_cast<ast::Func*>(node));
      break;
    case ast::VariableNode:
      print_variable(dynamic_cast<ast::Variable*>(node));
      break;
    case ast::ConstantNode:
      print_constant(dynamic_cast<ast::Constant*>(node));
      break;
    case ast::CastNode:
      print_cast(dynamic_cast<ast::Cast*>(node));
      break;
    case ast::UnOpNode:
      print_un_op(dynamic_cast<ast::UnOp*>(node));
      break;
    case ast::BinOpNode:
      print_bin_op(dynamic_cast<ast::BinOp*>(node));
      break;
    case ast::CallNode:
      print_call(dynamic_cast<ast::Call*>(node));
      break;
    case ast::ReturnNode:
      print_return(dynamic_cast<ast::Return*>(node));
      break;
    case ast::BranchNode:
      print_branch(dynamic_cast<ast::Branch*>(node));
      break;
    case ast::AssignmentNode:
      print_assignment(dynamic_cast<ast::Assignment*>(node));
      break;
    case ast::WhileNode:
      print_while(dynamic_cast<ast::While*>(node));
      break;
    case ast::VoidContextNode:
      print_void_context(dynamic_cast<ast::VoidContext*>(node));
      break;
    default:
      std::cout << "[Unknown Node] ??";
      return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
  }

  int program(ast::Program* program) {
    assert(program != nullptr);
    for (auto it = program->body.begin(); it != program->body.end(); ++it) {
      node(it->get());
    }
    return EXIT_SUCCESS;
  }
};

int pretty_print::node(ast::abstract::Node* node) {
  return PrettyPrinter().node(node);
}

int pretty_print::value(ast::types::abstract::Value* value) {
  return PrettyPrinter().value(value);
}

int pretty_print::program(ast::Program* program) {
  return PrettyPrinter().program(program);
}
