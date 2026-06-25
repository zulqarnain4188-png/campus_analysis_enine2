#include "filehandler.h"

// Simplest method to split a comma-separated line into cells
void manualSplit(const std::string& line, char delimiter, DataRow& row) {
    row.colCount = 0;
    std::string currentCell = "";
    
    for (size_t i = 0; i < line.length(); ++i) {
        if (line[i] == delimiter) {
            if (row.colCount < MAX_COLS) {
                row.cells[row.colCount++] = currentCell;
            }
            currentCell = "";
        } else {
            currentCell += line[i];
        }
    }
    if (row.colCount < MAX_COLS) {
        row.cells[row.colCount++] = currentCell;
    }
}

// Reads data, and AUTOMATICALLY creates the file if it's missing!
int readTXT(const std::string& filename, DataRow rows[]) {
    // Opening with app mode creates the file automatically if it doesn't exist
    std::fstream file(filename, std::ios::in | std::ios::out | std::ios::app);
    
    // Move file pointer back to the beginning to read it
    file.seekg(0, std::ios::beg);

    std::string line;
    // Skip the first header line
    if (!std::getline(file, line)) {
        file.close();
        return 0;
    }

    int count = 0;
    while (std::getline(file, line) && count < MAX_ROWS) {
        if (line.empty()) continue;
        manualSplit(line, ',', rows[count]);
        count++;
    }
    file.close();
    return count;
}

// Overwrites a file simply
bool writeTXT(const std::string& filename, const DataRow rows[], int rowCount, const std::string& header) {
    std::ofstream file(filename, std::ios::trunc); // Clear and open
    if (!file.is_open()) return false;

    file << header << "\n";
    for (int i = 0; i < rowCount; ++i) {
        for (int j = 0; j < rows[i].colCount; ++j) {
            file << rows[i].cells[j];
            if (j < rows[i].colCount - 1) file << ",";
        }
        file << "\n";
    }
    file.close();
    return true;
}

// Appends a row simply. If file is empty, writes a default header first!
bool appendTXT(const std::string& filename, const DataRow& row) {
    // Check if file is completely empty first
    std::ifstream checkFile(filename);
    bool isNew = (checkFile.peek() == std::ifstream::traits_type::eof());
    checkFile.close();

    std::ofstream file(filename, std::ios::app);
    if (!file.is_open()) return false;

    // Auto-generate safe matching headers if the file was just auto-created blank
    if (isNew) {
        if (filename == "students.txt") file << "roll,name,dept,cgpa,status\n";
        else if (filename == "courses.txt") file << "course_code,course_name,credit_hours,total_seats,prerequisite\n";
        else if (filename == "enrollments.txt") file << "roll,course_code,semester,status\n";
        else if (filename == "attendance_log.txt") file << "roll,course_code,date,status\n";
        else if (filename == "fees.txt") file << "roll,semester,amount_due,amount_paid,due_date,paid_date\n";
        else if (filename == "grades.txt") file << "roll,course_code,total_score,letter_grade\n";
    }

    // Write the actual row data
    for (int j = 0; j < row.colCount; ++j) {
        file << row.cells[j];
        if (j < row.colCount - 1) file << ",";
    }
    file << "\n";
    file.close();
    return true;
}

// Simple loop searching
int findRow(const DataRow rows[], int rowCount, int colIndex, const std::string& targetValue) {
    for (int i = 0; i < rowCount; ++i) {
        if (colIndex < rows[i].colCount && rows[i].cells[colIndex] == targetValue) {
            return i;
        }
    }
    return -1;
}

bool rowExists(const DataRow rows[], int rowCount, int colIndex, const std::string& targetValue) {
    return findRow(rows, rowCount, colIndex, targetValue) != -1;
}