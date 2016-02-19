#include "../assert.h"
#include "../syntax/ast.hpp"


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

%attribute callP     std::shared_ptr<ast::Call>
%constraint CALL callP 1 2

%attribute exprP     std::shared_ptr<ast::abstract::Operation>
%constraint EXPR exprP 1 2
%constraint EXPRESS exprP 0

%attribute statemP   std::shared_ptr<ast::abstract::Statement>
%constraint STATEMENT statemP 1 2
%constraint STATEMENTS statemP 0

%attribute declP     ast::Variable
%constraint DECLAR declP 1 2
%constraint DECLARS declP 0
%constraint ARGS  declP 0
%constraint ARGSS declP 0

%attribute topP     ast::Func
%constraint TOP topP 1 2
%constraint PROG topP 0

%attribute instP    instP_t
%constraint INSTR instP 1 2

%attribute typeP    ast::types::Type
%constraint TYPE typeP 1 2


%token EOF SCANERROR
%token SEMICOLON COLON COMMA
%token IDENTIFIER INT FLOAT
%token AND OR XOR LESS LESSEQ GREATER GREATEREQ EQ NOTEQ
%token PLUS TIMES MINUS DIVIDES MOD
%token LPAR RPAR
%token LSQPAR RSQPAR
%token LCURLY RCURLY
%token BOOL INT FLOAT STRING REF
%token TBOOL TINT TFLOAT TSTRING TREF
%token STRUCT FUN PROC RETURN
%token IF THEN ELSE FOR IN WHILE 


%token STATEMENT STATEMENTS
%token DECLAR DECLARS
%token TOP 
%token INSTR
%token DECL
%token EXPR
%token TYPE
%token FIELDS
%token ARGS ARGSS
%token EXPRESS
%token CALL
%token OP

%global memory varstore

%intokenheader #include "varstore.h"
%intokenheader #include "../syntax/ast.hpp"
%intokenheader #include "../syntax/pretty_print.hpp"
%intokenheader #include "../util/parser_misc.hpp"

%startsymbol PROG EOF


% PROG : TOP

    token p = tkn_PROG;
    p.topP = TOP1->topP;
    return p;

%      | PROG TOP

    PROG1->topP.push_back(TOP2->topP.front());
    return PROG1;

%      ;

% TYPE : TBOOL

    token t = tkn_TYPE;
    t.typeP.push_back(ast::types::Bool);
    return t;

%      | TINT

    token t = tkn_TYPE;
    t.typeP.push_back(ast::types::Int32);
    return t;

%      | TFLOAT

    token t = tkn_TYPE;
    t.typeP.push_back(ast::types::Float32);
    return t;

%      | TSTRING

    token t = tkn_TYPE;
    t.typeP.push_back(ast::types::String);
    return t;

%      ;

% TOP : FUN IDENTIFIER LPAR ARGSS RPAR COLON TYPE LCURLY INSTR RCURLY

    token t = tkn_TOP;
    ast::Func f;
    f.identifier = IDENTIFIER2->id.front();
    f.return_value = TYPE7->typeP.front();
    f.arguments = ARGSS4->declP;
    f.body = INSTR9->instP.front().second;
    f.local_variables = INSTR9->instP.front().first;
    t.topP.push_back(f);
    return t;

%     | PROC IDENTIFIER LPAR ARGSS RPAR LCURLY INSTR RCURLY

    token t = tkn_TOP;
    ast::Func f;
    f.identifier = IDENTIFIER2->id.front();
    f.return_value = ast::types::Void;
    f.arguments = ARGSS4->declP;
    f.body = INSTR7->instP.front().second;
    f.local_variables = INSTR7->instP.front().first;
    t.topP.push_back(f);
    return t;

%     ;


% INSTR : DECLARS STATEMENTS

    token i = tkn_INSTR;
    i.instP.push_back(instP_t(DECLARS1->declP, STATEMENTS2->statemP));
    return i;

%       | STATEMENTS

    token i = tkn_INSTR;
    i.instP.push_back(instP_t(std::list<ast::Variable>(), 
                STATEMENTS1->statemP));
    return i;

%       | DECLARS

    token i = tkn_INSTR;
    i.instP.push_back(instP_t(DECLARS1->declP, ast::Body()));
    return i;

%       |

    token i = tkn_INSTR;
    i.instP.push_back(instP_t(std::list<ast::Variable>(), ast::Body()));
    return i;

%       ;


% DECLARS : DECLAR SEMICOLON 

    token d = tkn_DECLARS;
    d.declP.push_back(DECLAR1->declP.front());
    return d;

%         | DECLARS DECLAR SEMICOLON

    DECLARS1->declP.push_back(DECLAR2->declP.front());
    return DECLARS1;

%         ;


// empty argument lists
% ARGSS : 

    token t = tkn_ARGSS;
    return t;

%       | ARGS

    ARGS1->type = tkn_ARGSS;
    return ARGS1;
    
%       ;


% ARGS : DECLAR 

    token a = tkn_ARGS;
    a.declP.push_back(DECLAR1->declP.front());
    return a;

%      | ARGS COMMA DECLAR

    ARGS1->declP.push_back(DECLAR3->declP.front());
    return ARGS1;

%      ;


% DECLAR : TYPE IDENTIFIER 

    token d = tkn_DECLAR;
    ast::Variable var(IDENTIFIER2->id.front(), TYPE1->typeP.front());
    d.declP.push_back(var);
    return d;

%        ;


% STATEMENTS : STATEMENT

    token s = tkn_STATEMENTS;
    s.statemP.push_back(STATEMENT1->statemP.front());
    return s;

%            | STATEMENTS STATEMENT

    STATEMENTS1->statemP.push_back(STATEMENT2->statemP.front());
    return STATEMENTS1;

%            ;


% STATEMENT : CALL SEMICOLON 

    token s = tkn_STATEMENT;
    ast::VoidContext vc(CALL1->callP.front());
    s.statemP.push_back(std::make_shared<ast::VoidContext>(vc));
    return s;

%      | IDENTIFIER EQ EXPR SEMICOLON

    token s = tkn_STATEMENT;
    ast::Assignment ass;
    ass.identifier = IDENTIFIER1->id.front();
    ass.value = EXPR3->exprP.front();
    s.statemP.push_back(std::make_shared<ast::Assignment>(ass));
    return s;

%      | IF LPAR EXPR RPAR LCURLY STATEMENTS RCURLY

    token s = tkn_STATEMENT;
    ast::Branch branch;
    branch.condition = EXPR3->exprP.front();
    branch.then_ = STATEMENTS6->statemP;
    s.statemP.push_back(std::make_shared<ast::Branch>(branch));
    return s;

%      | IF LPAR EXPR RPAR LCURLY STATEMENTS RCURLY ELSE LCURLY STATEMENTS RCURLY

    token s = tkn_STATEMENT;
    ast::Branch branch;
    branch.condition = EXPR3->exprP.front();
    branch.then_ = STATEMENTS6->statemP;
    branch.else_ = STATEMENTS10->statemP;
    s.statemP.push_back(std::make_shared<ast::Branch>(branch));
    return s;

%      | WHILE LPAR EXPR RPAR LCURLY STATEMENTS RCURLY

    token s = tkn_STATEMENT;
    ast::While w;
    w.condition = EXPR3->exprP.front();
    w.body = STATEMENTS6->statemP;
    s.statemP.push_back(std::make_shared<ast::While>(w));
    return s;

%      | RETURN EXPR SEMICOLON

    token s = tkn_STATEMENT;
    ast::Return r;
    r.value = EXPR2->exprP.front();
    s.statemP.push_back(std::make_shared<ast::Return>(r));
    return s;

%      ;


% EXPR : IDENTIFIER

    token e = tkn_EXPR;
    ast::Variable v(IDENTIFIER1->id.front());
    e.exprP.push_back(std::make_shared<ast::Variable>(v));
    return e;

%      | INT

    token e = tkn_EXPR;
    ast::types::IntValue iv(INT1->intV.front());
    ast::Constant c(std::make_shared<ast::types::IntValue>(iv));
    e.exprP.push_back(std::make_shared<ast::Constant>(c));
    return e;

%      | FLOAT

    token e = tkn_EXPR;
    ast::types::FloatValue fv(FLOAT1->floatV.front());
    ast::Constant c(std::make_shared<ast::types::FloatValue>(fv));
    e.exprP.push_back(std::make_shared<ast::Constant>(c));
    return e;

%      | BOOL

    token e = tkn_EXPR;
    ast::types::BoolValue bv(BOOL1->boolV.front());
    ast::Constant c(std::make_shared<ast::types::BoolValue>(bv));
    e.exprP.push_back(std::make_shared<ast::Constant>(c));
    return e;

%      | STRING

    token e = tkn_EXPR;
    ast::types::StringValue sv(STRING1->id.front());
    ast::Constant c(std::make_shared<ast::types::StringValue>(sv));
    e.exprP.push_back(std::make_shared<ast::Constant>(c));
    return e;

%      | CALL

    token e = tkn_EXPR;
    e.exprP.push_back(CALL1->callP.front());
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

%    | EQ EQ

    token op = tkn_OP;
    op.id.push_back("=");
    return op;

%    | NOTEQ

    token op = tkn_OP;
    op.id.push_back("!=");
    return op;

%    | MOD

    token op = tkn_OP;
    op.id.push_back("%");
    return op;

%    ;


% CALL : IDENTIFIER LPAR EXPRESS RPAR

    token call = tkn_CALL;
    ast::Call c;
    c.function_name = IDENTIFIER1->id.front();
    c.arguments = EXPRESS3->exprP;
    call.callP.push_back(std::make_shared<ast::Call>(c));
    return call;

%      | IDENTIFIER LPAR RPAR

    token call = tkn_CALL;
    ast::Call c;
    c.function_name = IDENTIFIER1->id.front();
    call.callP.push_back(std::make_shared<ast::Call>(c));
    return call;

%      ;

