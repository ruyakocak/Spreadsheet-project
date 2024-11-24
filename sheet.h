#ifndef SHEET_H
#define SHEET_H

#include "AnsiTerminal.h"
#include "cell.h"
class Spreadsheet
{
private:
    std::vector<std::vector<Cell>> chart;
public:
    Spreadsheet(int row = INIT_ROW, int col = INIT_COLUMN);
    ~Spreadsheet();
    int totalrows, totalcols;
    void resizes(int row, int coloumn);
    void printrows(AnsiTerminal &terminal, char starthere, int totalrow, int colcounter) const;
    void printcoloumns(AnsiTerminal &terminal, int startfrom, int totalcoloumn) const;
    void printchart(AnsiTerminal &terminal, int rowCounter, int colCounter, int currentRow, int currentCol);
    void start(int currentrows, int coloumns);
    Cell &getCell(int currentrow, int coloumn);
};

#endif