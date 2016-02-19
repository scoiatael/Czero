#ifndef VARSTORE_INCLUDED
#define VARSTORE_INCLUDED   1

#include <map>
#include <string>
#include <iostream>
#include <cmath>

#define _USE_MATH_DEFINES

class varstore
{
    std::map< std::string, double > values;

public:
    varstore();
    bool contains(const std::string& s) const;
    void assign(const std::string& s, double val);
    double lookup(const std::string& s) const;

    void clear();

    friend std::ostream& operator << (std::ostream&, const varstore&);
};

std::ostream& operator << (std::ostream&, const varstore& vs);

#endif
