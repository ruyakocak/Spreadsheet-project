#include "file.h"

void File::read_and_fill(const std::string &filename, Spreadsheet &sheet)
{
    std::ifstream file(filename); // Uses ifstream for file reading
    std::string line;
    int row = 0, col = 0;

    if (!file.is_open())
    {
        return; // If file couldn't open, exit
    }

    while (getline(file, line))
    {                               // Read lines from file
        std::stringstream ss(line); // Put line in a stringstream
        std::string deger;
        col = 0;

        // Grow number of row if not enough
        if (row >= INIT_ROW)
        {
            sheet.resizes(row + 2, INIT_COLUMN + 2); // Add new rows
        }

        // Read each comma-separated element in the line
        while (getline(ss, deger, ','))
        {
            // Hücreyi doldur
            sheet.getCell(row, col).setvalue(deger);

            col++;

            // Grow number of column if not enough
            if (col >= INIT_COLUMN)
            {
                sheet.resizes(row + 2, col + 2); // Add new columns
            }
        }
        row++; // One more line has been processed
    }

    file.close(); // Close the file
}

void File::save_file(Spreadsheet &sheet)
{
    std::ofstream file("saved.csv"); // Save value of cells to the saved.csv
    if (!file.is_open())
        return;
    for (int i = 0; i < sheet.totalrows; i++)
    {
        for (int j = 0; j < sheet.totalcols; j++)
        {
            if (j != sheet.totalcols - 1)
                file << sheet.getCell(i, j).getvalue() << ",";
            else // If it is the last element of line do not print comma
                file << sheet.getCell(i, j).getvalue() << std::endl;
        }
    }
    file.close();
}