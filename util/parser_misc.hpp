#ifndef PARSER_MISC_INCLUDED
#define PARSER_MISC_INCLUDED 1

#include "../syntax/ast.hpp"

typedef std::pair<std::list<ast::Variable>, ast::Body> instP_t;

template <typename T1, typename T2>
std::ostream& operator <<(std::ostream& stream, std::pair<T1, T2> const& p)
{
    return stream << p.first << " , " << p.second;
}

template <typename T>
std::ostream& operator <<(std::ostream& stream, std::list<T> const& l)
{
    for(auto& e : l)
        stream << e;
    return stream;
}

#endif
