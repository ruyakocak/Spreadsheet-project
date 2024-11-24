#ifndef CELL_H
#define CELL_H
#include <string>
class Cell
{
private:
    std::string value;
    std::string expression;

public:
    std::string &getvalue();
    void setexpression(std::string formula) { expression = formula; }
    const std::string &getexpression() const { return expression; }
    const std::string &getvalue() const;
    void setvalue(const std::string &val);
};

#endif