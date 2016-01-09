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
      r. moveforward( );
      if( r. lookahead == '=' )
      {
         r. moveforward( );

         lookahead. push_back( tkn_BECOMES );
         return;
      }

      // A single : is not a token.

      lookahead. push_back( tkn_SCANERROR );
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
      lookahead. push_back( tkn_IDENTIFIER );
      lookahead. back( ). id. push_back( std::string( ));
         // This appends an empty string to id, so that
         // we have a string attribute now.

      lookahead. back( ). id. back( ) += r. lookahead;
      r. moveforward( );

      while( isletter( r. lookahead ) || isdigit( r. lookahead ) ||
             r. lookahead == '_' )
      {
         lookahead. back( ). id. back( ) += r. lookahead;
         r. moveforward( );
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

   if( r. lookahead == '!' )
   {
      lookahead. push_back( tkn_FACTORIAL );
      r. moveforward( );
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
