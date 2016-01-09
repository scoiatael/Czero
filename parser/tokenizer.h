// -*-c++-*-
#ifndef TOKENIZER_INCLUDED
#define TOKENIZER_INCLUDED    1


#include <list>

#include "reader.h"
#include "token.h"


struct tokenizer
{
   reader r;

   std::list< token > lookahead;

   void scan( );
      // Gets a token from somewhere and appends it to lookahead.

   void syntaxerror( );
      // Report syntax error to the user.

};



#endif
