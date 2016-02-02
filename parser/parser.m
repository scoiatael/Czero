#include "../assert.h"
#include "../syntax/ast.hpp"
#include "../syntax/pretty_print.hpp"


%attribute id            std::string
%attribute reason        std::string
%attribute intval        int
%attribute floatval      float
%attribute callParam     ast::Call

%token EOF SCANERROR
%token SEMICOLON COLON COMMA
%token IDENTIFIER INT FLOAT
%token AND OR XOR LESS LESSEQ GREATER GREATEREQ EQ NOTEQ
%token PLUS TIMES MINUS DIVIDES
%token LPAR RPAR
%token LSQPAR RSQPAR
%token LCURLY RCURLY
%token BOOL INT FLOAT STRING REF
%token STRUCT FUN PROC
%token IF THEN ELSE FOR IN WHILE 


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

%constraint IDENTIFIER id 1 2

%constraint OP id 1 2

//%constraint CALL callParam 1 2

%global memory varstore

%intokenheader #include "varstore.h"
%intokenheader #include "../syntax/ast.hpp"
%intokenheader #include "../syntax/pretty_print.hpp"

%startsymbol OP EOF

// temporarily that's it
% TYPE : BOOL 
%      | INT 
%      | FLOAT 
%      | STRING 
%      ;

% PROG : TOP PROG
%      |
%      ;

% TOP : STRUCT IDENTIFIER LCURLY FIELDS RCURLY
%     | FUN IDENTIFIER LPAR ARGS RPAR COLON TYPE LCURLY INSTR RCURLY
%     | PROC IDENTIFIER LPAR ARGS RPAR LCURLY INSTR RCURLY
%     ;

// XXX - empty structs permitted
% FIELDS : TYPE IDENTIFIER SEMICOLON FIELDS
%       |
%       ;

// XXX - wrong
% ARGS : TYPE IDENTIFIER COMMA ARGS
%     |
%     ;

% INSTR : DECL
%       | DECL SEMICOLON INSTR
%       ;

% DECL : TYPE IDENTIFIER LPAR EXPRESS RPAR
%      | CALL 
%      | EXPR PLUS EQ EXPR
%      | IF EXPR LCURLY INSTR RCURLY
%      | FOR LPAR IDENTIFIER IN EXPR RPAR LCURLY INSTR RCURLY
%      | WHILE LPAR EXPR RPAR LCURLY INSTR RCURLY
%      ;

% EXPR : IDENTIFIER
    
%      | CALL
// fill missing
%      | EXPR OP EXPR
%      | LPAR EXPR RPAR
%      | IF EXPR THEN EXPR ELSE EXPR
%      ;

% EXPRESS : EXPR COLON EXPRESS
%         |
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

% CALL : IDENTIFIER LPAR EXPR RPAR
    //token call = tkn_CALL;
    //ast::Call c;
    //call.callParam.push_back(c);
    //return call;
%      ;

