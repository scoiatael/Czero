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

   if( r. lookahead == '"' ) // STRING
   {
       std::string s;
       r. moveforward();
       while(r.lookahead != '"')
       {
           if(r.lookahead == '\\') //escaped character
           {
               r. moveforward();
               s += r.lookahead;
           }
           else
           {
               s += r.lookahead;
           }
           r . moveforward();
       }
       lookahead.push_back( tkn_STRING );
       lookahead.back().id.push_back(s);
       r. moveforward();
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
      r. moveforward( );
      return;
   }


   if( r. lookahead == ',' )
   {
      lookahead. push_back( tkn_COMMA );
      r. moveforward( );
      return;
   }


   if( isletter( r. lookahead ) || r. lookahead == '_' )
   {
      std::string name;
      name.push_back(r.lookahead);

      r. moveforward( );

      while( isletter( r. lookahead ) || isdigit( r. lookahead ) ||
             r. lookahead == '_' )
      {
         name.push_back(r.lookahead);
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
         lookahead.push_back(tkn_TBOOL);
      else if(name == "int")
         lookahead.push_back(tkn_TINT);
      else if(name == "float")
         lookahead.push_back(tkn_TFLOAT);
      else if(name == "string")
         lookahead.push_back(tkn_TSTRING);
      else if(name == "ref")
         lookahead.push_back(tkn_TREF);
      else if(name == "struct")
         lookahead.push_back(tkn_STRUCT);
      else if(name == "fun")
         lookahead.push_back(tkn_FUN);
      else if(name == "proc")
         lookahead.push_back(tkn_PROC);
      else if(name == "return")
         lookahead.push_back(tkn_RETURN);
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
      else if(name == "true")
      {
         lookahead.push_back(tkn_BOOL);
         lookahead.back().boolV.push_back(true);
      }
      else if(name == "false")
      {
         lookahead.push_back(tkn_BOOL);
         lookahead.back().boolV.push_back(false);
      }
      else
      {
         lookahead.push_back(tkn_IDENTIFIER);
         lookahead.back().id.push_back(name);
      }

      return;
   }


   if( isdigit( r. lookahead ))
   {
      std::string s;
      bool isfloat = false;

      double val = 0.0;
      while( isdigit( r. lookahead ))
      {
         val = val * 10.0 + ( r. lookahead - '0' );

         s += r. lookahead;
         r. moveforward( );
      }

      if( r. lookahead == '.' )
      {
         isfloat = true;
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

     if(isfloat)
     {
         lookahead.push_back(tkn_FLOAT);
         lookahead.back().floatV.push_back(val);
     }
     else
     {
         lookahead.push_back(tkn_INT);
         lookahead.back().intV.push_back((int)val);
     }
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

   if( r. lookahead == '%' )
   {
      lookahead. push_back( tkn_MOD );
      r. moveforward( );
      return;
   }

   if( r. lookahead == '/' )
   {
      r. moveforward( );
      if( r. lookahead == '/' ) // comment
      {
          r. moveforward( );
          while(r. lookahead != '\n')
              r. moveforward( );
          r. moveforward( );
          scan();
          //std::cout << "after comment\n";
      }
      else 
      {
          lookahead. push_back( tkn_DIVIDES );
      }
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
