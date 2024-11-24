#include "sheet.h"
#include "formulaparser.h"
#include "file.h"

// Function to handle user input and update the spreadsheet accordingly
int handleInput(char key, Spreadsheet &sheet, formulaparser &parser, File &fileHandler, AnsiTerminal &terminal) {
    static int currentCol = 0;     // Tracks the current column position
    static int currentRow = 0;     // Tracks the current row position
    static int colCounter = 0;     // Tracks additional columns beyond initial setup
    static int rowCounter = 1;     // Tracks additional rows beyond initial setup
    static char columnStart = 'A'; // Tracks the starting column letter

    // Handle navigation and special keys
    if (key == '\x18' || key == '\x19' || key == '\x1A' || key == '\x1B' || key == '"') {
        switch (key) {
        case '\x18': // Arrow Up
            currentRow -= 1;
            break;
        case '\x19': // Arrow Down
            currentRow += 1;
            break;
        case '\x1A': // Arrow Right
            currentCol += 1;
            break;
        case '\x1B': // Arrow Left
            currentCol -= 1;
            break;
        case '\"':   // Save and exit
            fileHandler.save_file(sheet);
            terminal.clearScreen();
            return 0; // Exit program
        }

        // Handle column boundary conditions
        if (currentCol > INIT_COLUMN - 1) {
            currentCol--;
            colCounter++;
            columnStart = 'A' + colCounter;
            sheet.totalcols++;
            sheet.resizes(sheet.totalrows, sheet.totalcols);
            sheet.printrows(terminal, columnStart, sheet.totalcols, colCounter);
        } else if (currentCol < 0) {
            currentCol++;
            if (columnStart != 'A') {
                colCounter--;
                columnStart = 'A' + colCounter;
                sheet.printrows(terminal, columnStart, sheet.totalcols, colCounter);
            }
        } else if (currentCol <= INIT_COLUMN - 1) {
            sheet.printrows(terminal, columnStart, sheet.totalcols, colCounter);
        }

        // Handle row boundary conditions
        if (currentRow > INIT_ROW - 1) {
            currentRow--;
            rowCounter++;
            sheet.totalrows++;
            sheet.resizes(sheet.totalrows, sheet.totalcols);
            sheet.printcoloumns(terminal, rowCounter, sheet.totalrows);
        } else if (currentRow < 0) {
            currentRow++;
            if (rowCounter != 1) {
                rowCounter--;
                sheet.printcoloumns(terminal, rowCounter, sheet.totalrows);
            }
        }

        if (currentRow <= INIT_ROW - 1) {
            sheet.printcoloumns(terminal, rowCounter, sheet.totalrows);
        }
    } 
    // Handle backspace key (~) to remove the last character from a cell's value
    else if (key == '~') {
        auto &cell = sheet.getCell(currentRow + rowCounter - 1, currentCol + colCounter);
        if (!cell.getvalue().empty()) {
            cell.getvalue().pop_back(); // Remove the last character
        }
        sheet.printchart(terminal, rowCounter, colCounter, currentRow, currentCol);
    }
    // Handle regular character input
    else if (key != '\n') {
        auto &cell = sheet.getCell(currentRow + rowCounter - 1, currentCol + colCounter);
        std::string updatedValue = cell.getvalue() + key;
        cell.setvalue(updatedValue);
    }
    // Handle Enter key to finalize the expression
    else if (key == '\n') {
        auto &cell = sheet.getCell(currentRow + rowCounter - 1, currentCol + colCounter);
        cell.setexpression(cell.getvalue());
        cell.setvalue(""); // Clear the temporary value after setting the expression
        parser.parseGrid(sheet); // Recalculate the grid
    }

    // Recalculate the grid after each input
    parser.parseGrid(sheet);

    // Update the terminal display
    sheet.printchart(terminal, rowCounter, colCounter, currentRow, currentCol);
    return 1; // Continue the program
}

// Main function to initialize and run the spreadsheet program
int main() {
    AnsiTerminal terminal;
    terminal.clearScreen();

    // Initialize the spreadsheet, parser, and file handler
    Spreadsheet sheet;
    formulaparser parser;
    File fileHandler;

    // Print the initial spreadsheet layout
    sheet.printrows(terminal, 'A', sheet.totalcols, 0);
    sheet.printcoloumns(terminal, 1, sheet.totalrows);
    sheet.start(sheet.totalrows, sheet.totalcols);

    char key;
    int control = 0;

    // Uncomment this line to pre-fill the spreadsheet with data from a file
    //fileHandler.read_and_fill("fill.csv", sheet);

    // Main input loop
    while (true) {
        key = terminal.getSpecialKey(); // Get user input
        control = handleInput(key, sheet, parser, fileHandler, terminal);
        if (!control)
            return 0; // Exit if the user chooses to quit
    }

    // Save the final state and clear the terminal on exit
    fileHandler.save_file(sheet);
    terminal.clearScreen();
    return 0;
}
