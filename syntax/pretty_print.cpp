#include "pretty_print.hpp"

class PrettyPrinter {
  int indent = 0;

  std::ostream& print_indent(std::ostream& stream) {
    for (int i = 0; i < this->indent; i++) {
      stream << " ";
    }
    return stream;
  }

  std::ostream& print_func_header(std::ostream& stream, 
          const ast::Extern& extrn) {
    stream << extrn.identifier << "(";
    for (auto it = extrn.arguments.begin(); it != extrn.arguments.end(); ++it) {
      stream << it->identifier << " : ";
      type(stream, it->type);
      if((std::next(it)) != extrn.arguments.end()) {
        stream << ", ";
      }
    }
    stream << (extrn.variadic ? ", ..." : "") << ")" << " : ";
    type(stream, extrn.return_value);
    return stream;
  }

  std::ostream& print_func(std::ostream& stream, const ast::Func& func) {
    stream << "fun ";
    print_func_header(stream, dynamic_cast<const ast::Extern&>(func));
    stream << " {\n";
    this->indent = 2;
    print_indent(stream);
    stream <<  "vars {\n";
    this->indent += 2;
    for (auto it = func.local_variables.begin(); 
            it != func.local_variables.end(); ++it) {
      print_indent(stream);
      type(stream, it->type);
      stream << " " << it->identifier << ";\n";
    }
    this->indent -= 2;
    print_indent(stream);
    stream << "}\n";
    print_body(stream, func.body);
    stream << "}\n";
    this->indent = 0;
    return stream;
  }

  std::ostream& print_extern(std::ostream& stream, const ast::Extern& extrn) {
    stream << "extern ";
    print_func_header(stream, extrn);
    stream << ";\n";
    return stream;
  }

  std::ostream& print_variable(std::ostream& stream, 
          const ast::Variable& variable) {
    type(stream, variable.type);
    stream << " " << variable.identifier;
    return stream;
  }

  std::ostream& print_constant(std::ostream& stream,
          const ast::Constant& constant) {
    value(stream, dynamic_cast<const ast::types::abstract::Value&>
            (*constant.value.get()));
    return stream;
  }

  std::ostream& print_cast(std::ostream& stream, const ast::Cast& cast) {
    stream << "cast";
    type(stream, cast.to);
    stream << " ";
    node(stream, dynamic_cast<const ast::abstract::Node&>(*cast.value.get()));
    return stream;
  }

  std::ostream& print_un_op(std::ostream& stream, const ast::UnOp& un_op) {
    stream << un_op.operator_;
    stream << " ";
    node(stream, dynamic_cast<const ast::abstract::Node&>(*un_op.value.get()));
    return stream;
  }

  std::ostream& print_bin_op(std::ostream& stream, const ast::BinOp& bin_op) {
    stream << "(";
    node(stream, dynamic_cast<const ast::abstract::Node&>(*bin_op.left.get()));
    stream << ")" << bin_op.operator_ << "(";
    node(stream, dynamic_cast<const ast::abstract::Node&>(*bin_op.right.get()));
    stream << ")";
    return stream;
  }

  std::ostream& print_call(std::ostream& stream, const ast::Call& call) {
    stream << call.function_name << "(";
    for (auto it = call.arguments.begin(); it != call.arguments.end(); ++it) {
      node(stream, *(it->get()));
      if((std::next(it)) != call.arguments.end()) {
        stream << ", ";
      }
    }
    stream << ")";
    return stream;
  }

  std::ostream& print_return(std::ostream& stream, const ast::Return& retrn) {
    print_indent(stream);
    stream << "return ";
    node(stream, dynamic_cast<const ast::abstract::Node&>(*retrn.value.get()));
    stream << ";\n";
    return stream;
  }

  std::ostream& print_branch(std::ostream& stream, const ast::Branch& branch) {
    print_indent(stream);
    stream << "if(";
    node(stream, 
            dynamic_cast<const ast::abstract::Node&>(*branch.condition.get()));
    stream << ") {\n";
    this->indent += 2;
    print_body(stream, branch.then_);
    this->indent -= 2;
    stream << " }\n else {\n";
    this->indent += 2;
    print_body(stream, branch.else_);
    this->indent -= 2;
    stream << "}\n";
    return stream;
  }

  std::ostream& print_assignment(std::ostream& stream, 
          const ast::Assignment& assignment) {
    print_indent(stream);
    stream << assignment.identifier << " := ";
    node(stream, *assignment.value.get());
    stream << ";\n";
    return stream;
  }

  std::ostream& print_body(std::ostream& stream, ast::Body body) {
    for (auto it = body.begin(); it != body.end(); ++it) {
      node(stream, *(it->get()));
    }
    return stream;
  }

  std::ostream& print_while(std::ostream& stream, const ast::While& whil) {
    print_indent(stream);
    stream << "while(";
    node(stream, *whil.condition.get());
    stream << ") {\n";
    this->indent += 2;
    print_body(stream, whil.body);
    this->indent -= 2;
    stream << "}\n";
    return stream;
  }

  std::ostream& print_void_context(std::ostream& stream, 
          const ast::VoidContext& void_context) {
    print_indent(stream);
    node(stream, *void_context.operation.get());
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

  std::ostream& node_type(std::ostream& stream, ast::RealNodeType node_type) {
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
      return stream;
    }
    return stream;
  }

  std::ostream& type(std::ostream& stream, ast::types::Type type) {
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
    default:
      stream << "[Unknown Value]";
      return stream;
    }
    return stream;
  }

  std::ostream& value(std::ostream& stream, 
          const ast::types::abstract::Value& value) {
    const boost::regex esc("\n");
    const std::string rep("\\\\n");

    type(stream, value.type());
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
    default:
      stream << " ??";
      return stream;
    }
    return stream;
  }

  std::ostream& node(std::ostream& stream, const ast::abstract::Node& node) {
    switch(node) {
    case ast::ExternNode:
      print_extern(stream, dynamic_cast<const ast::Extern&>(node));
      break;
    case ast::FuncNode:
      print_func(stream, dynamic_cast<const ast::Func&>(node));
      break;
    case ast::VariableNode:
      print_variable(stream, dynamic_cast<const ast::Variable&>(node));
      break;
    case ast::ConstantNode:
      print_constant(stream, dynamic_cast<const ast::Constant&>(node));
      break;
    case ast::CastNode:
      print_cast(stream, dynamic_cast<const ast::Cast&>(node));
      break;
    case ast::UnOpNode:
      print_un_op(stream, dynamic_cast<const ast::UnOp&>(node));
      break;
    case ast::BinOpNode:
      print_bin_op(stream, dynamic_cast<const ast::BinOp&>(node));
      break;
    case ast::CallNode:
      print_call(stream, dynamic_cast<const ast::Call&>(node));
      break;
    case ast::ReturnNode:
      print_return(stream, dynamic_cast<const ast::Return&>(node));
      break;
    case ast::BranchNode:
      print_branch(stream, dynamic_cast<const ast::Branch&>(node));
      break;
    case ast::AssignmentNode:
      print_assignment(stream, dynamic_cast<const ast::Assignment&>(node));
      break;
    case ast::WhileNode:
      print_while(stream, dynamic_cast<const ast::While&>(node));
      break;
    case ast::VoidContextNode:
      print_void_context(stream, dynamic_cast<const ast::VoidContext&>(node));
      break;
    default:
      stream << "[Unknown Node] ??";
    }
    return stream;
  }

  std::ostream& program(std::ostream& stream, const ast::Program& program) {
    for (auto it = program.body.begin(); it != program.body.end(); ++it) {
      node(stream, *(it->get()));
    }
    return stream;
  }
};

std::ostream& pretty_print::node(std::ostream& stream,
        const ast::abstract::Node& node) {
  return PrettyPrinter().node(stream, node);
}

std::ostream& pretty_print::value(std::ostream& stream, 
        const ast::types::abstract::Value& value) {
  return PrettyPrinter().value(stream, value);
}

std::ostream& pretty_print::program(std::ostream& stream, 
        const ast::Program& program) {
  return PrettyPrinter().program(stream, program);
}

std::ostream& operator << (std::ostream& stream, 
        const ast::abstract::Node& node)
{
    return PrettyPrinter().node(stream, node);
}

std::ostream& operator << (std::ostream& stream, 
        const ast::types::abstract::Value& value)
{
    return PrettyPrinter().value(stream, value);
}

std::ostream& operator << (std::ostream& stream, 
        const ast::Program& program)
{
    return PrettyPrinter().program(stream, program);
}
