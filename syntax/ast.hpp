#ifndef AST_INCLUDED
#define AST_INCLUDED 1

#include <vector>
#include <string>
#include <memory>

namespace ast {
  int generate_new_id();

  namespace types {
    enum Type { Int32, Float32, Bool };

    namespace abstract {
      class Value {};
    };

    class BoolValue : public abstract::Value {
      bool value;
    };
    class FloatValue : public abstract::Value {
      float value;
    };
    class IntBalue : public abstract::Value {
      int value;
    };
  };

  enum RealNodeTypes {
    VariableNode,
    ExternNode,
    FuncNode,
    ConstantNode,
    CastNode,
    UnOpNode,
    BinOpNode,
    CallNode,
    ReturnNode,
    BranchNode,
    AssignmentNode,
    WhileNode
  };

  namespace abstract {
    class Node {
      // Id for this node
      int id { generate_new_id() };
      // Return RealNodeType for this node
      virtual operator int() = 0;
    };

    class Statement : public Node {};

    class Declaration : public Node {};

    class Operation : public Node {
      types::Type type;
    };
  }

  typedef std::string Identifier;

  typedef std::vector<std::unique_ptr<abstract::Statement>> Body;

  struct Variable : public abstract::Operation {
    Identifier identifier;
    operator int() { return VariableNode; }
  };

  struct Extern : public abstract::Declaration {
    types::Type return_value;
    std::vector<Variable> arguments;
    operator int() { return ExternNode; }
  };

  struct Func : public Extern {
    Body body;
    std::vector<Variable> variables;
    operator int() { return FuncNode; }
  };

  struct Constant : public abstract::Operation {
    types::abstract::Value value;
    operator int() { return ConstantNode; }
  };

  struct Cast : public abstract::Operation {
    types::Type to;
    std::unique_ptr<abstract::Operation> value;
    operator int() { return CastNode; }
  };

  struct UnOp : public abstract::Operation {
    Identifier operator_;
    std::unique_ptr<abstract::Operation> Body;
    operator int() { return UnOpNode; }
  };

  struct BinOp : public abstract::Operation {
    Identifier operator_;
    std::unique_ptr<abstract::Operation> left;
    std::unique_ptr<abstract::Operation> right;
    operator int() { return BinOpNode; }
  };

  struct Call : public abstract::Operation {
    Identifier function_name;
    std::vector<std::unique_ptr<abstract::Operation>> arguments;
    operator int() { return CallNode; }
  };

  struct Return : public abstract::Statement {
    std::unique_ptr<abstract::Operation> value;
    operator int() { return ReturnNode; }
  };

  struct Branch : public abstract::Statement {
    std::unique_ptr<abstract::Operation> condition;
    Body then_;
    Body else_;
    operator int() { return BranchNode; }
  };

  struct Assignment : public abstract::Statement {
    Identifier identifier;
    std::unique_ptr<abstract::Operation> value;
    operator int() { return AssignmentNode; }
  };

  struct While : public abstract::Statement {
    std::unique_ptr<abstract::Operation> condition;
    Body body;
    operator int() { return WhileNode; }
  };

  struct Program {
    std::vector<std::unique_ptr<abstract::Declaration>> body;
  };
}

#endif
