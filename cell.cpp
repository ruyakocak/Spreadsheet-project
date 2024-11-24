#include "cell.h"

// Const version of getvalue()
const std::string &Cell::getvalue() const
{
    return value;
}

// Non-const version of getvalue() to modify the value
std::string &Cell::getvalue()
{
    return value;
}

void Cell::setvalue(const std::string &val)
{
    value = val;
}
