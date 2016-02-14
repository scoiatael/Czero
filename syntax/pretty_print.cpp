#include "pretty_print.hpp"

class PrettyPrinter {
  int indent = 0;
  std::ostream& stream;

  std::ostream& print_indent() {
    for (int i = 0; i < this->indent; i++) {
      stream << " ";
    }
    return stream;
  }

  std::ostream& print_func_header(const ast::Extern& extrn) {
    stream << extrn.identifier << "(";
    for (auto it = extrn.arguments.begin(); it != extrn.arguments.end(); ++it) {
      stream << it->identifier << " : ";
      type(it->type);
      if((std::next(it)) != extrn.arguments.end()) {
        stream << ", ";
      }
    }
    stream << (extrn.variadic ? ", ..." : "") << ")" << " : ";
    type(extrn.return_value);
    return stream;
  }

  std::ostream& print_func(const ast::Func& func) {
    stream << "fun ";
    print_func_header(func);
    stream << " {\n";
    this->indent = 2;
    print_indent();
    stream <<  "vars {\n";
    this->indent += 2;
    for (auto it = func.local_variables.begin();
            it != func.local_variables.end(); ++it) {
      print_indent();
      type(it->type);
      stream << " " << it->identifier << ";\n";
    }
    this->indent -= 2;
    print_indent();
    stream << "}\n";
    print_body(func.body);
    stream << "}\n";
    this->indent = 0;
    return stream;
  }

  std::ostream& print_extern(const ast::Extern& extrn) {
    stream << "extern ";
    print_func_header(extrn);
    stream << ";\n";
    return stream;
  }

  std::ostream& print_variable(const ast::Variable& variable) {
    type(variable.type);
    stream << " " << variable.identifier;
    return stream;
  }

  std::ostream& print_constant(const ast::Constant& constant) {
    value(*constant.value);
    return stream;
  }

  std::ostream& print_cast(const ast::Cast& cast) {
    stream << "cast";
    type(cast.type);
    stream << " ";
    node(*cast.value);
    return stream;
  }

  std::ostream& print_un_op(const ast::UnOp& un_op) {
    stream << un_op.operator_;
    stream << " ";
    node(*un_op.value);
    return stream;
  }

  std::ostream& print_bin_op(const ast::BinOp& bin_op) {
    stream << "(";
    node(*bin_op.left);
    stream << ") `" << bin_op.operator_ << "` (";
    node(*bin_op.right);
    stream << ")";
    return stream;
  }

  std::ostream& print_call(const ast::Call& call) {
    stream << call.function_name << "(";
    for (auto it = call.arguments.begin(); it != call.arguments.end(); ++it) {
      node(**it);
      if((std::next(it)) != call.arguments.end()) {
        stream << ", ";
      }
    }
    stream << ")";
    return stream;
  }

  std::ostream& print_return(const ast::Return& retrn) {
    print_indent();
    stream << "return ";
    node(*retrn.value);
    stream << ";\n";
    return stream;
  }

  std::ostream& print_branch(const ast::Branch& branch) {
    print_indent();
    stream << "if(";
    node(*branch.condition);
    stream << ") {\n";
    this->indent += 2;
    print_body(branch.then_);
    this->indent -= 2;
    stream << " }\n else {\n";
    this->indent += 2;
    print_body(branch.else_);
    this->indent -= 2;
    stream << "}\n";
    return stream;
  }

  std::ostream& print_assignment(const ast::Assignment& assignment) {
    print_indent();
    stream << assignment.identifier << " := ";
    node(*assignment.value);
    stream << ";\n";
    return stream;
  }

  std::ostream& print_body(ast::Body body) {
    for (auto it = body.begin(); it != body.end(); ++it) {
      node(**it);
    }
    return stream;
  }

  std::ostream& print_while(const ast::While& whil) {
    print_indent();
    stream << "while(";
    node(*whil.condition);
    stream << ") {\n";
    this->indent += 2;
    print_body(whil.body);
    this->indent -= 2;
    print_indent();
    stream << "}\n";
    return stream;
  }

  std::ostream& print_void_context(const ast::VoidContext& void_context) {
    print_indent();
    node(*void_context.operation);
    stream << ";\n";
    return stream;
  }

  const ast::types::BoolValue& asBool(
          const ast::types::abstract::Value& value) {
    return dynamic_cast<const ast::types::BoolValue&>(value);
  }

  const ast::types::FloatValue& asFloat(
          const ast::types::abstract::Value& value) {
    return dynamic_cast<const ast::types::FloatValue&>(value);
  }

  const ast::types::IntValue& asInt(
          const ast::types::abstract::Value& value) {
    return dynamic_cast<const ast::types::IntValue&>(value);
  }

  const ast::types::StringValue& asString(
          const ast::types::abstract::Value& value) {
    return dynamic_cast<const ast::types::StringValue&>(value);
  }

public:
  PrettyPrinter() : stream(std::cout) {}
  PrettyPrinter(std::ostream& stream_) : stream(stream_) {}

  std::ostream& node_type(ast::RealNodeType node_type) {
    switch(node_type) {
    case ast::VariableNode:
      stream << "[VariableNode]";
      break;
    case ast::ExternNode:
      stream << "[ExternNode]";
      break;
    case ast::FuncNode:
      stream << "[FuncNode]";
      break;
    case ast::ConstantNode:
      stream << "[ConstantNode]";
      break;
    case ast::CastNode:
      stream << "[CastNode]";
      break;
    case ast::UnOpNode:
      stream << "[UnOpNode]";
      break;
    case ast::BinOpNode:
      stream << "[BinOpNode]";
      break;
    case ast::CallNode:
      stream << "[CallNode]";
      break;
    case ast::ReturnNode:
      stream << "[ReturnNode]";
      break;
    case ast::BranchNode:
      stream << "[BranchNode]";
      break;
    case ast::AssignmentNode:
      stream << "[AssignmentNode]";
      break;
    case ast::WhileNode:
      stream << "[WhileNode]";
      break;
    case ast::VoidContextNode:
      stream << "[VoidContextNode]";
      break;
    default:
      stream << "[Unknown Value]";
    }
    return stream;
  }

  std::ostream& type(ast::types::Type type) {
    switch(type) {
    case ast::types::Bool:
      stream << "[Bool]";
      break;
    case ast::types::Float32:
      stream << "[Float32]";
      break;
    case ast::types::Int32:
      stream << "[Int32]";
      break;
    case ast::types::String:
      stream << "[String]";
      break;
    case ast::types::Void:
      stream << "[Void]";
      break;
    default:
      stream << "[Unknown Value]";
    }
    return stream;
  }

  std::ostream& value(const ast::types::abstract::Value& value) {
    const boost::regex esc("\n");
    const std::string rep("\\\\n");

    type(value.type());
    stream << " ";
    switch(value) {
    case ast::types::Bool:
      stream << asBool(value).value;
      break;
    case ast::types::Float32:
      stream << asFloat(value).value;
      break;
    case ast::types::Int32:
      stream << asInt(value).value;
      break;
    case ast::types::String:
      stream << "\""
                << boost::regex_replace(asString(value).value, esc, rep,
                                        boost::match_default | boost::format_sed)
                << "\"";
      break;
    case ast::types::Void:
      stream << "()";
      break;
    default:
      stream << " ??";
    }
    return stream;
  }

  std::ostream& node(const ast::abstract::Node& node) {
    switch(node) {
    case ast::ExternNode:
      print_extern(dynamic_cast<const ast::Extern&>(node));
      break;
    case ast::FuncNode:
      print_func(dynamic_cast<const ast::Func&>(node));
      break;
    case ast::VariableNode:
      print_variable(dynamic_cast<const ast::Variable&>(node));
      break;
    case ast::ConstantNode:
      print_constant(dynamic_cast<const ast::Constant&>(node));
      break;
    case ast::CastNode:
      print_cast(dynamic_cast<const ast::Cast&>(node));
      break;
    case ast::UnOpNode:
      print_un_op(dynamic_cast<const ast::UnOp&>(node));
      break;
    case ast::BinOpNode:
      print_bin_op(dynamic_cast<const ast::BinOp&>(node));
      break;
    case ast::CallNode:
      print_call(dynamic_cast<const ast::Call&>(node));
      break;
    case ast::ReturnNode:
      print_return(dynamic_cast<const ast::Return&>(node));
      break;
    case ast::BranchNode:
      print_branch(dynamic_cast<const ast::Branch&>(node));
      break;
    case ast::AssignmentNode:
      print_assignment(dynamic_cast<const ast::Assignment&>(node));
      break;
    case ast::WhileNode:
      print_while(dynamic_cast<const ast::While&>(node));
      break;
    case ast::VoidContextNode:
      print_void_context(dynamic_cast<const ast::VoidContext&>(node));
      break;
    default:
      stream << "[Unknown Node] ??";
    }
    return stream;
  }

  std::ostream& program(const ast::Program& program) {
    for (auto it = program.body.begin(); it != program.body.end(); ++it) {
      node(**it);
    }
    return stream;
  }
};

std::ostream& pretty_print::type(std::ostream& stream,
                                 const ast::types::Type type) {
  return PrettyPrinter(stream).type(type);
}

std::ostream& pretty_print::node(std::ostream& stream,
                                 const ast::abstract::Node& node) {
  return PrettyPrinter(stream).node(node);
}

std::ostream& pretty_print::value(std::ostream& stream,
        const ast::types::abstract::Value& value) {
  return PrettyPrinter(stream).value(value);
}

std::ostream& pretty_print::program(std::ostream& stream,
        const ast::Program& program) {
  return PrettyPrinter(stream).program(program);
}

std::ostream& operator << (std::ostream& stream,
        const ast::abstract::Node& node) {
  return pretty_print::node(stream, node);
}

std::ostream& operator << (std::ostream& stream,
        const ast::types::abstract::Value& value) {
  return pretty_print::value(stream, value);
}

std::ostream& operator << (std::ostream& stream,
        const ast::Program& program) {
  return pretty_print::program(stream, program);
}
