#include "../assert.h"


%attribute id            std::string
%attribute reason        std::string
%attribute value         int

%token EOF SCANERROR
%token SEMICOLON BECOMES COMMA
%token IDENTIFIER NUMBER
%token PLUS TIMES MINUS DIVIDES
%token FACTORIAL
%token LPAR RPAR


%startsymbol S EOF


% S : S PLUS T
%   | T
%   ;


% T : T TIMES U
%   | U
%   ;

% U : IDENTIFIER
%   | LPAR S RPAR
%   ;
