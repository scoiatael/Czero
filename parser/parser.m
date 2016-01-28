#include "../assert.h"


%attribute id            std::string
%attribute reason        std::string
%attribute intval        int
%attribute floatval      float

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

%startsymbol PROG EOF

// temporary that's it
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
%    | MINUS
%    | TIMES
%    | DIVIDES
%    | AND
%    | OR
%    | XOR
%    | LESS
%    | LESSEQ
%    | GREATER
%    | GREATEREQ
%    | EQ
%    | NOTEQ
%    ;

% CALL : IDENTIFIER LPAR EXPR RPAR
%      ;

