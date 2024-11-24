
#ifndef ANSI_TERMINAL_H
#define ANSI_TERMINAL_H

#define col_width 11

#define INIT_ROW 21
#define INIT_COLUMN 8
#define NUMBER_OF_ALL_LETTERS 26

#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <unistd.h>  // For read()
#include <termios.h> // For terminal control

class AnsiTerminal
{
public:
    // Constructor: Sets up the terminal for capturing keystrokes
    AnsiTerminal();

    // Destructor: Restores the terminal settings to the original state
    ~AnsiTerminal();

    // Print text at a specified row and column (just col_width characters)
    void printAt(int row, int col, const std::string &text);

    // Print text at a specified row and column (all characters)
    void printAt(int row, int col, const std::string &text, int differ);

    //Print integer value
    void printAt(int row, int col, int value);

    //Print char value
    void printAt(int row, int col, char value);

    //For cursor, just clear the position (col_width characters)
    void printAt(int row, int col);

    //Print integer value inverted
    void printInvertedAt(int row, int col, int value);

    //Print char value inverted
    void printInvertedAt(int row, int col, char value);

    //For cursor, just clear the position (col_width characters) with inverted colors
    void printInvertedAt(int row, int col);

    // Print text with inverted background at a specified row and column
    void printInvertedAt(int row, int col, const std::string &text);

    // Clear the terminal screen
    void clearScreen();

    // Get a single keystroke from the terminal
    char getKeystroke();

    // Get the arrow key or special key input ('\x18', '\x19', '\x1A', '\x1B' for Up, Down, Left, Right)
    // or detect other key combinations such as Alt+Key, Ctrl+Key, etc.
    char getSpecialKey();

private:
    struct termios original_tio; // Holds the original terminal settings
};

#endif // ANSI_TERMINAL_H