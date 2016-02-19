#include "varstore.h"
#include "../assert.h"

varstore::varstore()
{
    values["pi"] = M_PI;
    //values["e"] = M_E;
}

bool varstore::contains(const std::string& s) const
{
    return values.find(s) != values.end();
}

void varstore::assign(const std::string& s, double val)
{
    values[s] = val;
}

double varstore::lookup(const std::string& s) const
{
    std::map< std::string, double > :: const_iterator p = values.find(s);
    ASSERT(p!=values.end());
    return p->second;
}

void varstore::clear()
{
    values.clear();
}

std::ostream& operator << (std::ostream& s, const varstore& vs)
{
    s << "varstore:\n";
    for(std::map< std::string, double > :: const_iterator p = 
            vs.values.begin(); p != vs.values.end(); p++)
        s << "  " << p->first << " is " << p->second << std::endl;
    s << std::endl;

    return s;
}
