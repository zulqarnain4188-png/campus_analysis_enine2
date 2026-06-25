#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <iostream>
#include <fstream>
#include <string>

// Maximum capacity for simple arrays since we aren't using complex STL containers
const int MAX_ROWS = 1000;
const int MAX_COLS = 20;

// Custom struct to represent a row of data dynamically without vectors/maps
struct DataRow {
    std::string cells[MAX_COLS];
    int colCount = 0;
};

// Required function signatures from specification
int readTXT(const std::string& filename, DataRow rows[]);
bool writeTXT(const std::string& filename, const DataRow rows[], int rowCount, const std::string& header);
bool appendTXT(const std::string& filename, const DataRow& row);
int findRow(const DataRow rows[], int rowCount, int colIndex, const std::string& targetValue);
bool rowExists(const DataRow rows[], int rowCount, int colIndex, const std::string& targetValue);

// Helper function to manually split strings without using standard template stream splitters
void manualSplit(const std::string& line, char delimiter, DataRow& row);

#endif