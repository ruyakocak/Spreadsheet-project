#include "sheet.h"
// Initialize all cells with default values
void Spreadsheet::start(int currentRows, int columns)
{
    for (int i = 0; i < currentRows; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            chart[i][j].setvalue("");
            chart[i][j].setexpression("");
        }
    }
}

// Constructor with specified dimensions
Spreadsheet::Spreadsheet(int currentRows, int columns)
{
    chart.resize(currentRows);
    for (int i = 0; i < currentRows; i++)
    {
        chart[i].resize(columns);
    }
    totalrows = currentRows;
    totalcols = columns;
    start(currentRows, columns); // Initialize all cells
}


Spreadsheet::~Spreadsheet(){
    chart.clear();
}


// Resize the spreadsheet dynamically
void Spreadsheet::resizes(int rows, int columns)
{
    if (rows > chart.size())
    {
        chart.resize(rows);
    }
    for (int i = 0; i < rows; i++)
    {
        if (columns > chart[i].size())
        {
            chart[i].resize(columns);
        }
    }
}

// Get a specific cell from the spreadsheet
Cell &Spreadsheet::getCell(int currentRow, int column)
{
    if (currentRow < 0 || currentRow >= chart.size() || column < 0 || column >= chart[currentRow].size())
    {
        throw std::out_of_range("Cell index out of bounds");
    }
    return chart[currentRow][column];
}

// Print row headers (column letters)
void Spreadsheet::printrows(AnsiTerminal &terminal, char startChar, int totalrow, int colcounter) const
{
    // Clear header row
    for (int i = 0; i < (col_width * INIT_COLUMN) + 4; i++)
    {
        terminal.printInvertedAt(1, i);
        terminal.printInvertedAt(2, i);
    }
    terminal.printInvertedAt(0, 0, "A1");

    for (int i = 0; i <= (col_width * INIT_COLUMN) + 3; i++)
    {
        terminal.printInvertedAt(4, i);
    }

    // Print column headers
    for (int col = 0; col < INIT_COLUMN; col++)
    {
        int currentColumn = col + colcounter;
        decltype(startChar) firstChar, secondChar; //Example decltype usage

        if (currentColumn < NUMBER_OF_ALL_LETTERS)
        {
            firstChar = 'A' + currentColumn;
            terminal.printInvertedAt(4, col * col_width + 8, firstChar);
        }
        else
        {
            firstChar = 'A' + (currentColumn / NUMBER_OF_ALL_LETTERS) - 1;
            secondChar = 'A' + (currentColumn % NUMBER_OF_ALL_LETTERS);
            terminal.printInvertedAt(4, col * col_width + 8, firstChar);
            terminal.printInvertedAt(4, col * col_width + 9, secondChar);
        }
    }
}

// Print column headers (row numbers)
void Spreadsheet::printcoloumns(AnsiTerminal &terminal, int startFrom, int totalcoloumn) const
{
    for (int i = 2; i < INIT_ROW + 2; i++)
    {
        terminal.printInvertedAt(i + 3, 1);
        terminal.printInvertedAt(i + 3, 2);
        terminal.printInvertedAt(i + 3, 3);
    }

    for (int i = 0; i < INIT_ROW; i++)
    {
        if (startFrom < 10)
        {
            terminal.printInvertedAt(i + 5, 1);
            terminal.printInvertedAt(i + 5, 2);
            terminal.printInvertedAt(i + 5, 3, startFrom);
        }
        else if (startFrom < 100)
        {
            terminal.printInvertedAt(i + 5, 1);
            terminal.printInvertedAt(i + 5, 2, startFrom);
        }
        else
        {
            terminal.printInvertedAt(i + 5, 1, startFrom);
        }
        startFrom++;
    }
}

// Print spreadsheet content
void Spreadsheet::printchart(AnsiTerminal &terminal, int rowCounter, int colCounter, int currentRow, int currentCol)
{
    // Clear all cells
    for (int i = 0; i < INIT_ROW; i++)
    {
        for (int j = 0; j < INIT_COLUMN; j++)
        {
            terminal.printAt(i + 5, col_width * j + 4);
        }
    }

    // Reprint all cells
    for (int i = 0; i < INIT_ROW; i++)
    {
        for (int j = 0; j < INIT_COLUMN; j++)
        {
            int actualRow = i + rowCounter - 1;
            int actualCol = j + colCounter;
            terminal.printAt(i + 5, col_width * j + 4, chart[actualRow][actualCol].getvalue());
        }
    }

    int actualRow = currentRow + rowCounter - 1; // Row counter comes 1 more than its value
    int actualCol = currentCol + colCounter;
    for(int i=0; i<col_width*INIT_COLUMN+4; i++)
        terminal.printAt(3, i, " "); // Clear previous selection
    terminal.printAt(3, 1, chart[actualRow][actualCol].getvalue(), 0);

    char firstChar, secondChar;

    if (actualCol < NUMBER_OF_ALL_LETTERS)
    { // Print updated rows and columns as inverted
        firstChar = 'A' + actualCol;
        terminal.printInvertedAt(1, 1, firstChar);
        terminal.printInvertedAt(1, 2, actualRow + 1);
    }
    else
    {
        firstChar = 'A' + (actualCol / NUMBER_OF_ALL_LETTERS) - 1;
        secondChar = 'A' + (actualCol % NUMBER_OF_ALL_LETTERS);
        terminal.printInvertedAt(1, 1, firstChar);
        terminal.printInvertedAt(1, 2, secondChar);
        terminal.printInvertedAt(1, 3, actualRow + 1);
    }

    for (int j = 0; j < col_width; j++)
    { // Print white screen to the place of current cell
        terminal.printInvertedAt(currentRow + 5, col_width * currentCol + 4 + j);
    }
    // Print the currently selected cell
    terminal.printInvertedAt(currentRow + 5, col_width * currentCol + 4, chart[currentRow + rowCounter - 1][currentCol + colCounter].getvalue());
}
