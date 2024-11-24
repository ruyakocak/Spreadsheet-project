#ifndef FILE_H
#define FILE_H
#include "sheet.h"

class File{
    public: 
        void read_and_fill(const std::string& filename, Spreadsheet& sheet); // Reads and fills the grid
        void save_file(Spreadsheet& sheet) ; //Saves values of cells to the csv file
};

#endif