#include "../assert.h"
#include "../syntax/ast.hpp"
#include "../syntax/pretty_print.hpp"


%attribute reason    std::string

%attribute id        std::string
%constraint IDENTIFIER id 1 2
%constraint OP         id 1 2
%constraint STRING     id 1 2

%attribute intV      int
%constraint INT intV 1 2

%attribute floatV    float
%constraint FLOAT floatV 1 2

%attribute boolV    bool
%constraint BOOL boolV 1 2

%attribute exprP     std::shared_ptr<ast::abstract::Operation>
%constraint EXPR exprP 1 2
%constraint EXPRESS exprP 0

%attribute statemP   std::shared_ptr<ast::abstract::Statement>
%constraint STATEMENT statemP 1 2
%constraint STATEMENTS statemP 0

%attribute declP     std::shared_ptr<ast::abstract::Declaration>

%attribute callP     ast::Call
%constraint CALL callP 1 2

%token EOF SCANERROR
%token SEMICOLON COLON COMMA
%token IDENTIFIER INT FLOAT
%token AND OR XOR LESS LESSEQ GREATER GREATEREQ EQ NOTEQ
%token PLUS TIMES MINUS DIVIDES
%token LPAR RPAR
%token LSQPAR RSQPAR
%token LCURLY RCURLY
%token BOOL INT FLOAT STRING REF
%token TBOOL TINT TFLOAT TSTRING TREF
%token STRUCT FUN PROC
%token IF THEN ELSE FOR IN WHILE 


%token STATEMENT
%token STATEMENTS
%token TOP
%token INSTR
%token DECL
%token EXPR
%token TYPE
%token FIELDS
%token ARGS
%token EXPRESS
%token CALL
%token OP

%global memory varstore

%intokenheader #include "varstore.h"
%intokenheader #include "../syntax/ast.hpp"
%intokenheader #include "../syntax/pretty_print.hpp"

%startsymbol EXPRESS EOF

% TYPE : TBOOL 
%      | TINT 
%      | TFLOAT 
%      | TSTRING 
%      | TREF
%      ;

% PROG : TOP
%      ;

% TOP : STRUCT IDENTIFIER LCURLY FIELDS RCURLY
%     | FUN IDENTIFIER LPAR ARGS RPAR COLON TYPE LCURLY INSTR RCURLY
%     | PROC IDENTIFIER LPAR ARGS RPAR LCURLY INSTR RCURLY
%     ;

// XXX - empty structs permitted
% FIELDS : TYPE IDENTIFIER SEMICOLON FIELDS
%        |
%        ;


% ARGS : TYPE IDENTIFIER COMMA ARGS
%     | TYPE IDENTIFIER

    token a = tkn_ARGS;

    return a;

%     ;

% INSTR : DECLARS STATEMENTS
%       | STATEMENTS
%       | DECLARS
%       ;

% STATEMENTS : STATEMENT

    token s = tkn_STATEMENTS;
    s.statemP.push_back(STATEMENT1->statemP.front());
    return s;

%            | STATEMENTS SEMICOLON STATEMENT

    STATEMENTS1->statemP.push_back(STATEMENT3->statemP.front());
    return STATEMENTS1;

%            ;

% STATEMENT : CALL 

    token s = tkn_STATEMENT;
    ast::VoidContext vc;
    vc.operation = std::make_shared<ast::Call>(CALL1->callP.front());
    s.statemP.push_back(std::make_shared<ast::VoidContext>(vc));
    return s;

%      | IDENTIFIER PLUS EQ EXPR

    token s = tkn_STATEMENT;
    ast::Assignment ass;
    ast::Variable var;
    ast::BinOp binop;

    var.identifier = IDENTIFIER1->id.front();

    binop.operator_ = PLUS2->id.front();
    binop.left = std::make_shared<ast::Variable>(var);
    binop.right = EXPR4->exprP.front();

    ass.identifier = var.identifier;
    ass.value = std::make_shared<ast::BinOp>(binop);

    s.statemP.push_back(std::make_shared<ast::Assignment>(ass));
    return s;

%      | IF EXPR LCURLY STATEMENTS RCURLY

    token s = tkn_STATEMENT;
    ast::Branch branch;
    branch.condition = EXPR2->exprP.front();
    branch.then_ = STATEMENTS4->statemP;
    return s;

%      | WHILE LPAR EXPR RPAR LCURLY STATEMENTS RCURLY

    token s = tkn_STATEMENT;
    ast::While w;
    w.condition = EXPR3->exprP.front();
    w.body = STATEMENTS6->statemP;
    return s;

%      ;

% EXPR : IDENTIFIER

    token e = tkn_EXPR;
    ast::Variable v;
    v.identifier = IDENTIFIER1->id.front();
    e.exprP.push_back(std::make_shared<ast::Variable>(v));
    return e;

%      | INT

    token e = tkn_EXPR;
    ast::Constant c;
    ast::types::IntValue iv;
    iv.value = INT1->intV.front();
    c.value = std::make_shared<ast::types::IntValue>(iv);
    e.exprP.push_back(std::make_shared<ast::Constant>(c));
    return e;

%      | FLOAT

    token e = tkn_EXPR;
    ast::Constant c;
    ast::types::FloatValue fv;
    fv.value = FLOAT1->floatV.front();
    c.value = std::make_shared<ast::types::FloatValue>(fv);
    e.exprP.push_back(std::make_shared<ast::Constant>(c));
    return e;

%      | BOOL

    token e = tkn_EXPR;
    ast::Constant c;
    ast::types::BoolValue bv;
    bv.value = BOOL1->boolV.front();
    c.value = std::make_shared<ast::types::BoolValue>(bv);
    e.exprP.push_back(std::make_shared<ast::Constant>(c));
    return e;

%      | STRING

    token e = tkn_EXPR;
    ast::Constant c;
    ast::types::StringValue sv;
    sv.value = STRING1->id.front();
    c.value = std::make_shared<ast::types::StringValue>(sv);
    e.exprP.push_back(std::make_shared<ast::Constant>(c));
    return e;

%      | CALL

    token e = tkn_EXPR;
    e.exprP.push_back(std::make_shared<ast::Call>(CALL1->callP.front()));
    return e;

%      | EXPR OP EXPR

    token e = tkn_EXPR;
    ast::BinOp binop;
    binop.operator_ = OP2->id.front();
    binop.left = EXPR1->exprP.front();
    binop.right = EXPR3->exprP.front();
    e.exprP.push_back(std::make_shared<ast::BinOp>(binop));
    return e;

%      | LPAR EXPR RPAR

    return EXPR2;

%      ;

% EXPRESS : EXPRESS COMMA EXPR

    EXPRESS1->exprP.push_back(EXPR3->exprP.front());
    return EXPRESS1;

%         | EXPR

    token e = tkn_EXPRESS;
    e.exprP.push_back(EXPR1->exprP.front());
    pretty_print::node(std::cout, *e.exprP.front().get()); 
    return e;

%         ;

% OP : PLUS

    token op = tkn_OP;
    op.id.push_back("+");
    return op;

%    | MINUS

    token op = tkn_OP;
    op.id.push_back("-");
    return op;

%    | TIMES

    token op = tkn_OP;
    op.id.push_back("*");
    return op;

%    | DIVIDES

    token op = tkn_OP;
    op.id.push_back("/");
    return op;

%    | AND

    token op = tkn_OP;
    op.id.push_back("and");
    return op;

%    | OR

    token op = tkn_OP;
    op.id.push_back("or");
    return op;

%    | XOR

    token op = tkn_OP;
    op.id.push_back("xor");
    return op;

%    | LESS

    token op = tkn_OP;
    op.id.push_back("<");
    return op;

%    | LESSEQ

    token op = tkn_OP;
    op.id.push_back("<=");
    return op;

%    | GREATER

    token op = tkn_OP;
    op.id.push_back(">");
    return op;

%    | GREATEREQ

    token op = tkn_OP;
    op.id.push_back(">=");
    return op;

%    | EQ

    token op = tkn_OP;
    op.id.push_back("=");
    return op;

%    | NOTEQ

    token op = tkn_OP;
    op.id.push_back("!=");
    return op;

%    ;

% CALL : IDENTIFIER LPAR EXPRESS RPAR

    token call = tkn_CALL;
    ast::Call c;
    c.function_name = IDENTIFIER1->id.front();
    c.arguments = EXPRESS3->exprP;
    call.callP.push_back(c);
    return call;

%      | IDENTIFIER LPAR RPAR

    token call = tkn_CALL;
    ast::Call c;
    c.function_name = IDENTIFIER1->id.front();
    call.callP.push_back(c);
    return call;

%      ;

