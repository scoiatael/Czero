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
      struct Value {
        virtual ~Value() {}
        virtual Type type() = 0;
        operator int() { return this->type(); }
      };
    };

    struct BoolValue : public abstract::Value {
      bool value;
      Type type() { return Bool; }
    };
    struct FloatValue : public abstract::Value {
      float value;
      Type type() { return Float32; }
    };
    struct IntValue : public abstract::Value {
      int value;
      Type type() { return Int32; }
      };
    struct StringValue : public abstract::Value {
      std::string value;
      Type type() { return String; }
    };
  };

  enum RealNodeType {
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
    struct Node {
      virtual ~Node() {}
      // Id for this node
      int id { generate_new_id() };
      virtual RealNodeType node_type() = 0;
      operator int() { return this->node_type(); }
    };

    struct Statement : public Node {
      virtual ~Statement() {}
    };

    struct Declaration : public Node {
      virtual ~Declaration() {}
    };

    struct Operation : public Node {
      virtual ~Operation() {}
      types::Type type;
    };
  }

  typedef std::string Identifier;

  typedef std::vector<std::shared_ptr<abstract::Statement>> Body;

  struct Variable : public abstract::Operation {
    Identifier identifier;
    RealNodeType node_type() { return VariableNode; }
  };

  struct Extern : public abstract::Declaration {
    Identifier identifier;
    types::Type return_value;
    std::vector<Variable> arguments;
    bool variadic;
    RealNodeType node_type() { return ExternNode; }
  };

  struct Func : public Extern {
    Body body;
    std::vector<Variable> local_variables;
    RealNodeType node_type() { return FuncNode; }
  };

  struct Constant : public abstract::Operation {
    std::shared_ptr<types::abstract::Value> value;
    RealNodeType node_type() { return ConstantNode; }
  };

  struct Cast : public abstract::Operation {
    types::Type to;
    std::shared_ptr<abstract::Operation> value;
    RealNodeType node_type() { return CastNode; }
  };

  struct UnOp : public abstract::Operation {
    Identifier operator_;
    std::shared_ptr<abstract::Operation> value;
    RealNodeType node_type() { return UnOpNode; }
  };

  struct BinOp : public abstract::Operation {
    Identifier operator_;
    std::shared_ptr<abstract::Operation> left;
    std::shared_ptr<abstract::Operation> right;
    RealNodeType node_type() { return BinOpNode; }
  };

  struct Call : public abstract::Operation {
    Identifier function_name;
    std::vector<std::shared_ptr<abstract::Operation>> arguments;
    RealNodeType node_type() { return CallNode; }
  };

  struct Return : public abstract::Statement {
    std::shared_ptr<abstract::Operation> value;
    RealNodeType node_type() { return ReturnNode; }
  };

  struct Branch : public abstract::Statement {
    std::shared_ptr<abstract::Operation> condition;
    Body then_;
    Body else_;
    RealNodeType node_type() { return BranchNode; }
  };

  struct Assignment : public abstract::Statement {
    Identifier identifier;
    std::shared_ptr<abstract::Operation> value;
    RealNodeType node_type() { return AssignmentNode; }
  };

  struct While : public abstract::Statement {
    std::shared_ptr<abstract::Operation> condition;
    Body body;
    RealNodeType node_type() { return WhileNode; }
  };

  struct VoidContext : public abstract::Statement {
    std::shared_ptr<abstract::Operation> operation;
    RealNodeType node_type() { return VoidContextNode; }
  };

  struct Program {
    std::vector<std::shared_ptr<abstract::Declaration>> body;
  };
}

#endif
