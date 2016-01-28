#include "tokenizer.h"


namespace
{

   inline bool isletter( char c )
   {
      if( c >= 'a' && c <= 'z' )
         return true;
      if( c >= 'A' && c <= 'Z' )
         return true;
      return false;
   }


   inline bool isdigit( char c )
   {
      if( c >= '0' && c <= '9' )
         return true;
      return false;
   }

}


void tokenizer::scan( )
{

   // Ignore whitespace:

   while( r. lookahead == ' ' || r. lookahead == '\n' ||
          r. lookahead == '\t' )
   {
      r. moveforward( );
   }


   if( r. lookahead == EOF )
   {
      lookahead. push_back( tkn_EOF );
      return;
   }

   if( r. lookahead == ';' )
   {
      lookahead. push_back( tkn_SEMICOLON );
      r. moveforward( );
      return;
   }


   if( r. lookahead == ':' )
   {
      lookahead. push_back( tkn_COLON );
      return;
   }


   if( r. lookahead == ',' )
   {
      r. moveforward( );
      lookahead. push_back( tkn_COMMA );
      return;
   }


   if( isletter( r. lookahead ) || r. lookahead == '_' )
   {
      std::string name;
      name.push_back(r.lookahead);

      //lookahead. push_back( tkn_IDENTIFIER );
      //lookahead. back( ). id. push_back( std::string( ));
         // This appends an empty string to id, so that
         // we have a string attribute now.

      //lookahead. back( ). id. back( ) += r. lookahead;
      r. moveforward( );

      while( isletter( r. lookahead ) || isdigit( r. lookahead ) ||
             r. lookahead == '_' )
      {
         name.push_back(r.lookahead);
         //lookahead. back( ). id. back( ) += r. lookahead;
         r.moveforward();
      }

      // #cant_into_macros
      if(name == "and")
         lookahead.push_back(tkn_AND);
      else if(name == "or")
         lookahead.push_back(tkn_OR);
      else if(name == "xor")
         lookahead.push_back(tkn_XOR);
      else if(name == "bool")
         lookahead.push_back(tkn_BOOL);
      else if(name == "int")
         lookahead.push_back(tkn_INT);
      else if(name == "float")
         lookahead.push_back(tkn_FLOAT);
      else if(name == "string")
         lookahead.push_back(tkn_STRING);
      else if(name == "ref")
         lookahead.push_back(tkn_REF);
      else if(name == "struct")
         lookahead.push_back(tkn_STRUCT);
      else if(name == "fun")
         lookahead.push_back(tkn_FUN);
      else if(name == "proc")
         lookahead.push_back(tkn_PROC);
      else if(name == "if")
         lookahead.push_back(tkn_IF);
      else if(name == "then")
         lookahead.push_back(tkn_THEN);
      else if(name == "else")
         lookahead.push_back(tkn_ELSE);
      else if(name == "for")
         lookahead.push_back(tkn_FOR);
      else if(name == "in")
         lookahead.push_back(tkn_IN);
      else if(name == "while")
         lookahead.push_back(tkn_WHILE);
      else
      {
         lookahead.back().id.push_back(name);
         lookahead.push_back(tkn_IDENTIFIER);
      }

      return;
   }


   if( isdigit( r. lookahead ))
   {
      std::string s;
         // We keep the string, so that we can put it in
         // a scanerror, if necessary.

      // We do not allow the number to start with + or -, because
      // this would bring us in conflict with the operators + and -.

      double val = 0.0;
      while( isdigit( r. lookahead ))
      {
         val = val * 10.0 + ( r. lookahead - '0' );

         s += r. lookahead;
         r. moveforward( );
      }

      if( r. lookahead == '.' )
      {
         double pos = 0.1;

         s += r. lookahead;
         r. moveforward( );

         if( !isdigit( r. lookahead ))
         {
            s += r. lookahead;
            r. moveforward( );

            lookahead. push_back( tkn_SCANERROR );
            lookahead. back( ). reason. push_back(s);
            return;
         }

         while( isdigit( r. lookahead ))
         {
            val += pos * ( r. lookahead - '0' );
            pos /= 10.0;

            s += r. lookahead;
            r. moveforward( );
         }
      }

      if( r. lookahead == 'e' || r. lookahead == 'E' )
      {
         unsigned int ee = 0;
         int polarity = 1;

         s += r. lookahead;
         r. moveforward( );

         if( r. lookahead == '+' || r. lookahead == '-' )
         {
            if( r. lookahead == '-' )
               polarity = -1;

            s += r. lookahead;
            r. moveforward( );
         }

         if( !isdigit( r. lookahead ))
         {
            s += r. lookahead;
            r. moveforward( );

            lookahead. push_back( tkn_SCANERROR );
            lookahead. back( ). reason. push_back(s);
            return;
         }

         while( isdigit( r. lookahead ))
         {
            ee = ee * 10 + ( r. lookahead - '0' );

            s += r. lookahead;
            r. moveforward( );
         }

         // We need to put val = val * 10^( polarity * ee ):

         // Is this efficient? Probably not, but it is simple.

         if( polarity == 1 )
         {
            while( ee )
            {
               val *= 10.0;
               -- ee;
            }
         }
         else
         {
            while( ee )
            {
               val /= 10.0;
               -- ee;
            }
         }

     }

     lookahead. push_back( tkn_NUMBER );
     lookahead. back( ). value. push_back( val );
     return;
   }

   if( r. lookahead == '+' )
   {
      lookahead. push_back( tkn_PLUS );
      r. moveforward( );
      return;
   }

   if( r. lookahead == '-' )
   {
      lookahead. push_back( tkn_MINUS );
      r. moveforward( );
      return;
   }

   if( r. lookahead == '*' )
   {
      lookahead. push_back( tkn_TIMES );
      r. moveforward( );
      return;
   }

   if( r. lookahead == '/' )
   {
      lookahead. push_back( tkn_DIVIDES );
      r. moveforward( );
      return;
   }

   if( r. lookahead == '<' )
   {
      r. moveforward( );
      if ( r. lookahead == '=' )
      {
          r. moveforward( );
          lookahead. push_back( tkn_LESSEQ );
          return;
      }
      lookahead. push_back( tkn_LESS );
      return;
   }

   if( r. lookahead == '>' )
   {
      r. moveforward( );
      if ( r. lookahead == '=' )
      {
          r. moveforward( );
          lookahead. push_back( tkn_GREATEREQ );
          return;
      }
      lookahead. push_back( tkn_GREATER );
      return;
   }

   if ( r. lookahead == '=' )
   {
      r. moveforward( );
      lookahead. push_back( tkn_EQ );
      return;
   }

   if( r. lookahead == '!' )
   {
      r. moveforward( );
      if ( r. lookahead == '=' )
      {
          r. moveforward( );
          lookahead. push_back( tkn_NOTEQ );
          return;
      }
      lookahead. push_back( tkn_SCANERROR );
      return;
   }

   if( r. lookahead == '(' )
   {
      lookahead. push_back( tkn_LPAR );
      r. moveforward( );
      return;
   }

   if( r. lookahead == ')' )
   {
      lookahead. push_back( tkn_RPAR );
      r. moveforward( );
      return;
   }

   if( r. lookahead == '[' )
   {
      lookahead. push_back( tkn_LSQPAR );
      r. moveforward( );
      return;
   }

   if( r. lookahead == ']' )
   {
      lookahead. push_back( tkn_RSQPAR );
      r. moveforward( );
      return;
   }
   if( r. lookahead == '{' )
   {
      lookahead. push_back( tkn_LCURLY );
      r. moveforward( );
      return;
   }

   if( r. lookahead == '}' )
   {
      lookahead. push_back( tkn_RCURLY );
      r. moveforward( );
      return;
   }

   // If we could not recognize anything, then we produce
   // a scan error.

   lookahead. push_back( tkn_SCANERROR );

   std::string s;
   if( r. lookahead != EOF )
   {
      s += r. lookahead;
      r. moveforward( );
         // It is important that we always advance the reader,
         // because otherwise the tokenizer hangs.
   }
   else
   {
      s += "eof";   // This should actually be unreachable, because
                    // EOF is checked in the beginning.
   }
   lookahead. back( ). reason. push_back(s);
   return;
}


void tokenizer::syntaxerror( )
{
   std::cout << "Syntax error in line " << r. linenumber << "\n";
}
