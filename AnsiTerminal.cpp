#include "AnsiTerminal.h"


// Constructor: Configure terminal for non-canonical mode
AnsiTerminal::AnsiTerminal() {
    // Save the original terminal settings
    tcgetattr(STDIN_FILENO, &original_tio);
    struct termios new_tio = original_tio;

    // Disable canonical mode and echo for real-time input reading
    new_tio.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);
}

// Destructor: Restore the terminal settings to original state
AnsiTerminal::~AnsiTerminal() {
    tcsetattr(STDIN_FILENO, TCSANOW, &original_tio);
}

// Method to print text at a specified position
void AnsiTerminal::printAt(int row, int col, const std::string &text) {
    std::cout << "\033[" << row << ";" << col << "H" << text.substr(0,col_width) << std::flush;
}

void AnsiTerminal::printAt(int row, int col, const std::string &text, int differ) { //Overloaded function for printing more than 9 characters
    std::cout << "\033[" << row << ";" << col << "H" << text << std::flush;
}


void AnsiTerminal::printAt(int row, int col, int value) {
    std::cout << "\033[" << row << ";" << col << "H" << value << std::flush;
}

void AnsiTerminal::printAt(int row, int col) {
    // Put cursor at specified row and column
    for(int i=0; i<col_width; i++)
        std::cout << "\033[" << row << ";" << col+i << "H"<< " ";

    // Reset color modes
    std::cout << "\033[0m" << std::flush;
}

void AnsiTerminal::printAt(int row, int col, char value) {
    std::cout << "\033[" << row << ";" << col << "H" << value << std::flush;
}



// Method to print text with inverted background at a specified position
void AnsiTerminal::printInvertedAt(int row, int col, const std::string &text) {
    std::cout << "\033[" << row << ";" << col << "H\033[7m" << text.substr(0,col_width) << "\033[0m" << std::flush;
    // \033[7m enables reverse video mode, \033[0m resets to normal
}


void AnsiTerminal::printInvertedAt(int row, int col, int value) {
    std::cout << "\033[" << row << ";" << col << "H\033[7m" << value << "\033[0m" << std::flush;
}

void AnsiTerminal::printInvertedAt(int row, int col, char value) {
    std::cout << "\033[" << row << ";" << col << "H\033[7m" << value << "\033[0m" << std::flush;
}

void AnsiTerminal::printInvertedAt(int row, int col) {
    std::cout << "\033[" << row << ";" << col << "H\033[7m \033[0m" << std::flush;
    // A space (" ") is written and only a white background is created with inverse color mode.
}




// Method to clear the terminal screen
void AnsiTerminal::clearScreen() {
    std::cout << "\033[2J\033[H" << std::flush; // Clear screen and move cursor to home
}

// Method to get a single keystroke from the terminal
char AnsiTerminal::getKeystroke() {
    char ch;
    read(STDIN_FILENO, &ch, 1);
    
    // We check the possibility that the Enter key will appear as both '\x0A' (LF) and '\x0D' (CR)
    if (ch == '\x0A' || ch == '\x0D') {
        return '\n';  // Enter olarak kabul edip '\n' döndürüyoruz
    }

    if(ch == 0x7F) return '~';
    if (ch >= 1 && ch <= 26) {
        return ch + 'A' - 1;
    }

    return ch;
}

char AnsiTerminal::getSpecialKey() {
    char ch = getKeystroke();

    if (ch == '\033') {
        char next_ch;
        if (read(STDIN_FILENO, &next_ch, 1) == 0) return '\033';

        if (next_ch == '[') {
            char arrow_key;
            if (read(STDIN_FILENO, &arrow_key, 1) == 1) {
                switch (arrow_key) {
                    case 'A': return '\x18'; //Up
                    case 'B': return '\x19'; //Down
                    case 'C': return '\x1A'; //Left
                    case 'D': return '\x1B'; //Right
                }
            }
        } else {
            return next_ch | 0x80;
        }
    }

    return ch;
}
