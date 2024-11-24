#ifndef FORMULAPARSER_H
#define FORMULAPARSER_H
#include <string>
#include <vector>
#include "sheet.h"
class formulaparser
{
public:
    void parseGrid(Spreadsheet &sheet);
    void calculate(std::vector<double> elemans, std::vector<char> signs, Cell &currentcell);
    int columnNameToIndex(const std::string &columnName);
    double safeStringToDouble(const std::string &str);
    int safeStringToInt(const std::string &str);
    const char* findChar(const char* str, char ch);
    double computeRangeFunction(const std::string &function, const std::string &range, Spreadsheet &sheet);
    std::string resolveFunctions(const std::string &expression, Spreadsheet &sheet);
    void ensureCellBounds(int row, int col, Spreadsheet &sheet);
    std::pair<int, int> parseCellReference(const std::string &cellRef);
};
#endif