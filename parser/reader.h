// -*-c++-*-
#ifndef READER_INCLUDED
#define READER_INCLUDED  1

#include <cstdio>
#include <iostream>


// This is a very simple reader that reads only from standard
// input.


struct reader
{
   char lookahead;

   unsigned int linenumber;


   reader( );

   bool hasmore( ) const;
      // If hasmore( ) == false, then lookahead is not a reliable
      // character anymore.

   void moveforward( );

};



inline reader::reader( )
{
   lookahead = getchar( );
   linenumber = 1;
}


inline bool reader::hasmore( ) const
{
   return lookahead != EOF;
}


inline void reader::moveforward( )
{
   if( hasmore( ))
   {
      if( lookahead == '\n' )
         ++ linenumber;
      lookahead = getchar( );
   }
}



#endif
