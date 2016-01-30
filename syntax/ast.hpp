#ifndef AST_INCLUDED
#define AST_INCLUDED 1

#include <vector>
#include <string>
#include <memory>

namespace ast {
  int generate_new_id();

  namespace types {
    enum Type { Poison, Int32, Float32, Bool, String };

    namespace abstract {
      class Value {};
    };

    class BoolValue : public abstract::Value {
      bool value;
    };
    class FloatValue : public abstract::Value {
      float value;
    };
    class IntValue : public abstract::Value {
      int value;
    };
    class StringValue : public abstract::Value {
      std::string value;
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
    WhileNode,
    VoidContextNode
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

  typedef std::vector<std::shared_ptr<abstract::Statement>> Body;

  struct Variable : public abstract::Operation {
    Identifier identifier;
    operator int() { return VariableNode; }
  };

  struct Extern : public abstract::Declaration {
    Identifier identifier;
    types::Type return_value;
    std::vector<Variable> arguments;
    bool variadic;
    operator int() { return ExternNode; }
  };

  struct Func : public Extern {
    Body body;
    operator int() { return FuncNode; }
  };

  struct Constant : public abstract::Operation {
    types::abstract::Value value;
    operator int() { return ConstantNode; }
  };

  struct Cast : public abstract::Operation {
    types::Type to;
    std::shared_ptr<abstract::Operation> value;
    operator int() { return CastNode; }
  };

  struct UnOp : public abstract::Operation {
    Identifier operator_;
    std::shared_ptr<abstract::Operation> Body;
    operator int() { return UnOpNode; }
  };

  struct BinOp : public abstract::Operation {
    Identifier operator_;
    std::shared_ptr<abstract::Operation> left;
    std::shared_ptr<abstract::Operation> right;
    operator int() { return BinOpNode; }
  };

  struct Call : public abstract::Operation {
    Identifier function_name;
    std::vector<std::shared_ptr<abstract::Operation>> arguments;
    operator int() { return CallNode; }
  };

  struct Return : public abstract::Statement {
    std::shared_ptr<abstract::Operation> value;
    operator int() { return ReturnNode; }
  };

  struct Branch : public abstract::Statement {
    std::shared_ptr<abstract::Operation> condition;
    Body then_;
    Body else_;
    operator int() { return BranchNode; }
  };

  struct Assignment : public abstract::Statement {
    Identifier identifier;
    std::shared_ptr<abstract::Operation> value;
    operator int() { return AssignmentNode; }
  };

  struct While : public abstract::Statement {
    std::shared_ptr<abstract::Operation> condition;
    Body body;
    operator int() { return WhileNode; }
  };

  struct VoidContext : public abstract::Statement {
    std::shared_ptr<abstract::Operation> operation;
    operator int() { return VoidContextNode; }
  };

  struct Program {
    std::vector<std::shared_ptr<abstract::Declaration>> body;
  };
}

#endif
