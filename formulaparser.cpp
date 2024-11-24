#include "formulaparser.h"
#include <cmath>
#include <sstream>
#include <algorithm>
#include <numeric>
#define ASCII_OF_A 65

// Convert column name (e.g., "A") to a 0-based index
int formulaparser::columnNameToIndex(const std::string &columnName)
{
    int colIndex = 0;
    for (char ch : columnName)
    {
        colIndex = colIndex * 26 + (ch - ASCII_OF_A + 1); // Convert 'A' to 1, 'B' to 2, etc.
    }
    return colIndex - 1; // Convert to 0-based index
}

// Parse a cell identifier like "A1" into row and column indices
std::pair<int, int> formulaparser::parseCellReference(const std::string &cellRef)
{
    size_t pos = 0;

    // Extract column part (e.g., "A")
    while (pos < cellRef.size() && isalpha(cellRef[pos]))
    {
        pos++;
    }

    std::string colPart = cellRef.substr(0, pos);
    std::string rowPart = cellRef.substr(pos);

    int col = columnNameToIndex(colPart);   // Convert column to 0-based index
    int row = safeStringToInt(rowPart) - 1; // Convert row to 0-based index

    return {row, col};
}

// Ensure the spreadsheet has enough space to accommodate a referenced cell
void formulaparser::ensureCellBounds(int row, int col, Spreadsheet &sheet)
{
    if (row >= sheet.totalrows || col >= sheet.totalcols)
    {
        int newRows = std::max(sheet.totalrows, row + 1);
        int newCols = std::max(sheet.totalcols, col + 1);
        sheet.resizes(newRows, newCols);
    }
}

// Convert string to double safely; return 0.0 if conversion fails
double formulaparser::safeStringToDouble(const std::string &str)
{
    try
    {
        return std::stod(str); // std::stod handles double conversion
    }
    catch (...)
    {
        return 0.0;
    }
}

int formulaparser::safeStringToInt(const std::string &str)
{
    try
    {
        return std::stoi(str); // std::stod handles double conversion
    }
    catch (...)
    {
        return 0.0;
    }
}

const char* formulaparser:: findChar(const char* str, char ch) {
    while (*str != '\0') {       // Traverse the string until the null terminator
        if (*str == ch) {        // If the current character matches
            return str;          // Return a pointer to this character
        }
        str++;
    }
    return nullptr;              // Return nullptr if the character is not found
}


// Parse range functions like MAX, MIN, SUM, etc., and return the computed result
double formulaparser::computeRangeFunction(const std::string &function, const std::string &range, Spreadsheet &sheet)
{
    // Check if the range has a valid format with a column letter
    bool validFormat = false;
    for (char ch : range)
    {
        if (isalpha(ch))
        {
            validFormat = true;
            break;
        }
    }

    // If range is invalid or doesn't contain a column letter, return 0
    if (!validFormat)
    {
        return 0.0;
    }

    // Check for valid range separator
    size_t separator = range.find("..");
    if (separator == std::string::npos)
    {
        return 0.0;
    }

    std::string startCell = range.substr(0, separator);
    std::string endCell = range.substr(separator + 2);

    // Validate cell references have column letters
    if (startCell.empty() || endCell.empty() || 
        !isalpha(startCell[0]) || !isalpha(endCell[0]))
    {
        return 0.0;
    }

    auto [startRow, startCol] = parseCellReference(startCell);
    auto [endRow, endCol] = parseCellReference(endCell);

    // Additional bounds checking
    if (startRow < 0 || endRow < 0 || startCol < 0 || endCol < 0)
    {
        return 0.0;
    }

    // Ensure bounds
    try 
    {
        ensureCellBounds(startRow, startCol, sheet);
        ensureCellBounds(endRow, endCol, sheet);
    }
    catch (...)
    {
        return 0.0;
    }

    // Gather non-empty values in the range
    std::vector<double> values;
    for (int i = startRow; i <= endRow; i++)
    {
        for (int j = startCol; j <= endCol; j++)
        {
            std::string cellValue = sheet.getCell(i, j).getvalue();
            // Only add non-empty values
            if (!cellValue.empty())
            {
                values.push_back(safeStringToDouble(cellValue));
            }
        }
    }

    // Return 0 if no valid values
    if (values.empty())
    {
        return 0.0;
    }

    // Perform the specified function
    if (function == "SUM" || function == "sum")
    {
        return std::accumulate(values.begin(), values.end(), 0.0);
    }
    else if (function == "AVER" || function == "aver")
    {
        return std::accumulate(values.begin(), values.end(), 0.0) / values.size();
    }
    else if (function == "STDDEV" || function == "stddev")
    {
        if (values.size() < 2)
            return 0.0;
        double mean = std::accumulate(values.begin(), values.end(), 0.0) / values.size();
        double variance = 0.0;
        for (double val : values)
        {
            variance += (val - mean) * (val - mean);
        }
        variance /= values.size() - 1;
        return std::sqrt(variance);
    }
    else if (function == "MAX" || function == "max")
    {
        return *std::max_element(values.begin(), values.end());
    }
    else if (function == "MIN" || function == "min")
    {
        return *std::min_element(values.begin(), values.end());
    }

    return 0.0;
}

// Parse the spreadsheet grid for formulas
void formulaparser::parseGrid(Spreadsheet &sheet)
{
    for (int i = 0; i < sheet.totalrows; i++)
    {
        for (int j = 0; j < sheet.totalcols; j++)
        {
            std::string expression = sheet.getCell(i, j).getexpression();
            if (!expression.empty() && expression[0] == '=')
            {
                std::string resolvedExpression = resolveFunctions(expression.substr(1), sheet);

                std::vector<double> elements;
                std::vector<char> operators;
                std::string operand;
                bool parsingNumber = false;

                // Parse the resolved expression after replacing function calls
                for (size_t k = 0; k < resolvedExpression.size(); ++k)
                {
                    char currentChar = resolvedExpression[k];

                    if (findChar("+-/*", currentChar))
                    {
                        // Process the accumulated operand before the operator
                        if (!operand.empty())
                        {
                            if (isalpha(operand[0]))
                            {
                                // Handle cell reference
                                auto [row, col] = parseCellReference(operand);
                                ensureCellBounds(row, col, sheet);
                                double cellValue = safeStringToDouble(sheet.getCell(row, col).getvalue());
                                elements.push_back(cellValue);
                            }
                            else
                            {
                                // Handle numeric value
                                elements.push_back(safeStringToDouble(operand));
                            }
                            operand.clear();
                        }
                        operators.push_back(currentChar);
                        parsingNumber = false;
                    }
                    else
                    {
                        // Accumulate the operand
                        operand += currentChar;
                    }
                }

                // Process the last operand
                if (!operand.empty())
                {
                    if (isalpha(operand[0]))
                    {
                        auto [row, col] = parseCellReference(operand);
                        ensureCellBounds(row, col, sheet);
                        double cellValue = safeStringToDouble(sheet.getCell(row, col).getvalue());
                        elements.push_back(cellValue);
                    }
                    else
                    {
                        elements.push_back(safeStringToDouble(operand));
                    }
                }

                // Perform the calculations
                if (!elements.empty())
                {
                    calculate(elements, operators, sheet.getCell(i, j));
                }
            }
        }
    }
}

// Resolves functions (SUM, MAX, MIN, etc.) and evaluates them in the expression
std::string formulaparser::resolveFunctions(const std::string &expression, Spreadsheet &sheet)
{
    std::string result = expression;
    size_t pos = 0;

    while (pos < result.length())
    {
        size_t funcStart = result.find_first_of("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz", pos);
        if (funcStart == std::string::npos)
            break;

        // Check if this is actually a function and not just a cell reference
        size_t funcEnd = result.find('(', funcStart);
        if (funcEnd == std::string::npos)
        {
            pos = funcStart + 1;
            continue;
        }

        std::string function = result.substr(funcStart, funcEnd - funcStart);

        // Find matching closing parenthesis
        size_t rangeEnd = funcEnd;
        int parenCount = 1;
        while (parenCount > 0 && ++rangeEnd < result.length())
        {
            if (result[rangeEnd] == '(')
                parenCount++;
            if (result[rangeEnd] == ')')
                parenCount--;
        }

        if (parenCount > 0)
            break; // Mismatched parentheses

        std::string range = result.substr(funcEnd + 1, rangeEnd - funcEnd - 1);

        // Only process if it's actually a range function
        if (function == "SUM" || function == "sum" || function == "MAX" || function == "max" || function == "MIN" ||
             function == "min" || function == "AVER" || function == "aver" || function == "STDDEV"|| function == "stddev")
        {
            // Compute the value of the function
            double computedValue = computeRangeFunction(function, range, sheet);

            // Replace the function call with its computed value
            std::ostringstream oss;
            oss << computedValue;
            result.replace(funcStart, rangeEnd - funcStart + 1, oss.str());

            pos = funcStart + oss.str().length();
        }
        else
        {
            pos = funcStart + 1; // Skip this occurrence and continue searching
        }
    }

    return result;
}
// Perform arithmetic calculations on the current cell
void formulaparser::calculate(std::vector<double> elements, std::vector<char> operators, Cell &currentcell)
{
    for (size_t i = 0; i < operators.size();)
    {
        if (operators[i] == '*' || operators[i] == '/')
        {
            double result = (operators[i] == '*') ? elements[i] * elements[i + 1] : (elements[i + 1] != 0 ? elements[i] / elements[i + 1] : 0);
            elements[i] = result;
            elements.erase(elements.begin() + i + 1);
            operators.erase(operators.begin() + i);
        }
        else
        {
            i++;
        }
    }

    double result = elements[0];
    for (size_t i = 0; i < operators.size(); i++)
    {
        result += (operators[i] == '+') ? elements[i + 1] : -elements[i + 1];
    }

    // Set the final result to the current cell
    std::ostringstream oss;
    oss << result;
    currentcell.setvalue(oss.str());
}