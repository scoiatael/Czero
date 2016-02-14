#ifndef AST_INCLUDED
#define AST_INCLUDED 1

#include <list>
#include <string>
#include <memory>

namespace ast {
  int generate_new_id();

  namespace types {
    enum Type { Poison, Void, Int32, Float32, Bool, String };

    namespace abstract {
      struct Value {
        virtual ~Value() {}
        virtual Type type() const = 0;
        operator int() const { return this->type(); }
      };
    };

    struct BoolValue : public abstract::Value {
      bool value;
      Type type() const { return Bool; }
      explicit BoolValue(bool value_) : value(value_) {}
      BoolValue() = default;
    };
    struct FloatValue : public abstract::Value {
      float value;
      Type type() const { return Float32; }
      explicit FloatValue(float value_) : value(value_) {}
      FloatValue() = default;
    };
    struct IntValue : public abstract::Value {
      int value;
      Type type() const { return Int32; }
      explicit IntValue(int value_) : value(value_) {}
      IntValue() = default;
      };
    struct StringValue : public abstract::Value {
      std::string value;
      Type type() const { return String; }
      explicit StringValue(std::string value_) : value(value_) {}
      StringValue() = default;
    };
    struct VoidValue : public abstract::Value {
      Type type() const { return Void; }
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
      virtual RealNodeType node_type() const = 0;
      operator int() const { return this->node_type(); }
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

  typedef std::list<std::shared_ptr<abstract::Statement>> Body;

  struct Variable : public abstract::Operation {
    Identifier identifier;
    RealNodeType node_type() const { return VariableNode; }
    explicit Variable(Identifier identifier_, ast::types::Type type_ = ast::types::Poison)
      : identifier(identifier_) {
      this->type = type_;
    }
    Variable() = default;
  };

  struct Extern : public abstract::Declaration {
    Identifier identifier;
    types::Type return_value;
    std::list<Variable> arguments;
    bool variadic;
    RealNodeType node_type() const { return ExternNode; }
  };

  struct Func : public Extern {
    Body body;
    std::list<Variable> local_variables;
    RealNodeType node_type() const { return FuncNode; }
  };

  struct Constant : public abstract::Operation {
    std::shared_ptr<types::abstract::Value> value;
    RealNodeType node_type() const { return ConstantNode; }
    explicit Constant(std::shared_ptr<types::abstract::Value> value_)
      : value(value_) {
      this->type = value_->type();
    }
    Constant() = default;
  };

  struct Cast : public abstract::Operation {
    std::shared_ptr<abstract::Operation> value;
    RealNodeType node_type() const { return CastNode; }
  };

  struct UnOp : public abstract::Operation {
    Identifier operator_;
    std::shared_ptr<abstract::Operation> value;
    RealNodeType node_type() const { return UnOpNode; }
  };

  struct BinOp : public abstract::Operation {
    Identifier operator_;
    types::Type from;
    std::shared_ptr<abstract::Operation> left;
    std::shared_ptr<abstract::Operation> right;
    RealNodeType node_type() const { return BinOpNode; }
  };

  struct Call : public abstract::Operation {
    Identifier function_name;
    std::list<std::shared_ptr<abstract::Operation>> arguments;
    RealNodeType node_type() const { return CallNode; }
  };

  struct Return : public abstract::Statement {
    std::shared_ptr<abstract::Operation> value;
    RealNodeType node_type() const { return ReturnNode; }
  };

  struct Branch : public abstract::Statement {
    std::shared_ptr<abstract::Operation> condition;
    Body then_;
    Body else_;
    RealNodeType node_type() const { return BranchNode; }
  };

  struct Assignment : public abstract::Statement {
    Identifier identifier;
    std::shared_ptr<abstract::Operation> value;
    RealNodeType node_type() const { return AssignmentNode; }
  };

  struct While : public abstract::Statement {
    std::shared_ptr<abstract::Operation> condition;
    Body body;
    RealNodeType node_type() const { return WhileNode; }
  };

  struct VoidContext : public abstract::Statement {
    std::shared_ptr<abstract::Operation> operation;
    RealNodeType node_type() const { return VoidContextNode; }
    explicit VoidContext(std::shared_ptr<abstract::Operation> operation_)
      : operation(operation_) {}
    VoidContext() = default;
  };

  struct Program {
    std::list<std::shared_ptr<abstract::Declaration>> body;
  };
}

#endif
