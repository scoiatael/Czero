// -*-c++-*-
#ifndef READER_INCLUDED
#define READER_INCLUDED  1

#include <iostream>

struct reader
{
  int lookahead;
  unsigned int linenumber = 1;
  std::istream& stream;

  reader(std::istream& stream_) : stream(stream_) {
    lookahead = stream.get( );
  }

  bool hasmore( ) const;
  // If hasmore( ) == false, then lookahead is not a reliable
  // character anymore.
  void moveforward( );
};


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
      lookahead = stream.get();
    }
}



#endif
