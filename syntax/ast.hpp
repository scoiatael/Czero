#ifdef AST_INCLUDED
#define AST_INCLUDED 1

#include <vector>
#include <string>

namespace ast {

    static int static_counter = 0;
    int generate_new_id()
    {
        return static_counter++;
    }

    enum Type { Int32, Float32, Bool };

    class Node {
        int id { generate_new_id() };
    };

    class Statement : public Node {
    };

    class Declaration : public Node {

    };

    class Operation : public Node {
        Type type;
    };

    typedef std::string Identifier;

    class Variable : public Operation {
        Identifier identifier;
    };

    class Extern : public Declaration {
        Type return_value;
        std::vector<Variable> arguments;
    };

    typedef std::vector<Statement> Body;

    class Func : public Declaration {
        Body body;
        std::vector<Variable> variables;
    };

    class Value {};

    class Constant : public Operation {
        Value value;
    };

    class Cast : public Operation {
        Type to;
        Operation value;
    };

    class UnOp : public Operation {
        Identifier operator_;
        Operation Body;
    };
    
    class BinOp : public Operation {
        Identifier operator_;
        Operation left;
        Operation right;
    };

    class Return : public Operation {
        Operation value;
    }; 

    class Call : public Operation {
        Identifier function_name;
        std::vector<Operation> arguments;
    };

    class Branch : public Statement {
        Operation condition;
        Body then_;
        Body else_;
    };

    class Assignment : public Statement {
        Identifier identifier;
        Operation value;
    };

    class While : public Statement {
        Operation condition;
        Body body;
    };

    class Program {
        Body body;
    };


    class BoolValue : public Value {
        bool value;
    };
    class FloatValue : public Value {
        float value;
    };
    class IntBalue : public Value {
        int value;
    };
}

#endif
