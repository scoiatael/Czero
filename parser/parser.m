#include "../assert.h"


%attribute id            std::string
%attribute reason        std::string
%attribute value         int

%token EOF SCANERROR
%token SEMICOLON BECOMES COMMA
%token IDENTIFIER NUMBER
%token PLUS TIMES MINUS DIVIDES
%token LPAR RPAR


%startsymbol PROG EOF


% PROG : TOP PROG
%   |
%   ;

