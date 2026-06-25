#include "student_ops.h"

// Validates format: BSAI-YY-XXX manually
bool validateRollFormat(const std::string& roll) {
    if (roll.length() != 11) return false;
    
    // Check "BSAI-"
    if (roll.substr(0, 5) != "BSAI-") return false;
    
    // Check "YY" are digits
    if (roll[5] < '0' || roll[5] > '9' || roll[6] < '0' || roll[6] > '9') return false;
    
    // Check dash
    if (roll[7] != '-') return false;
    
    // Check "XXX" are digits
    if (roll[8] < '0' || roll[8] > '9' || 
        roll[9] < '0' || roll[9] > '9' || 
        roll[10] < '0' || roll[10] > '9') {
        return false;
    }
    
    return true;
}

// Validates that name has absolutely no digits
bool validateName(const std::string& name) {
    if (name.empty()) return false;
    for (size_t i = 0; i < name.length(); ++i) {
        if (name[i] >= '0' && name[i] <= '9') {
            return false;
        }
    }
    return true;
}

// Adds a student profile after performing strict manual validations
bool addStudent(const std::string& roll, const std::string& name, const std::string& dept, const std::string& cgpa) {
    if (!validateRollFormat(roll) || !validateName(name)) {
        std::cerr << "Validation Failed: Invalid roll format or name contains numbers.\n";
        return false;
    }

    DataRow dataset[MAX_ROWS];
    int count = readTXT("students.txt", dataset);

    // Strict Rule Check: No duplicates allowed
    if (rowExists(dataset, count, 0, roll)) {
        std::cerr << "Error: Student with Roll Number " << roll << " already exists.\n";
        return false;
    }

    // Prepare data row matching columns: roll, name, dept, cgpa, status
    DataRow newRow;
    newRow.cells[0] = roll;
    newRow.cells[1] = name;
    newRow.cells[2] = dept;
    newRow.cells[3] = cgpa;
    newRow.cells[4] = "active";
    newRow.colCount = 5;

    return appendTXT("students.txt", newRow);
}

// Searches and prints a specific active student by Roll Number
void searchByRoll(const std::string& roll) {
    DataRow dataset[MAX_ROWS];
    int count = readTXT("students.txt", dataset);
    
    int index = findRow(dataset, count, 0, roll);
    if (index != -1 && dataset[index].cells[4] == "active") {
        std::cout << "\n--- Student Found ---\n";
        std::cout << "Roll No:    " << dataset[index].cells[0] << "\n";
        std::cout << "Name:       " << dataset[index].cells[1] << "\n";
        std::cout << "Department: " << dataset[index].cells[2] << "\n";
        std::cout << "CGPA:       " << dataset[index].cells[3] << "\n";
        std::cout << "Status:     " << dataset[index].cells[4] << "\n---------------------\n";
    } else {
        std::cout << "Student record not found or inactive.\n";
    }
}

// Implements structural substring filtering for prefix matching
void searchByName(const std::string& nameSubstr) {
    DataRow dataset[MAX_ROWS];
    int count = readTXT("students.txt", dataset);
    bool foundAny = false;

    std::cout << "\n--- Search Results Matching Prefix ---\n";
    for (int i = 0; i < count; ++i) {
        if (dataset[i].cells[4] != "active") continue;

        std::string currentName = dataset[i].cells[1];
        
        // Manual substring search logic (No forbidden algorithm imports)
        if (currentName.length() >= nameSubstr.length()) {
            if (currentName.substr(0, nameSubstr.length()) == nameSubstr) {
                std::cout << dataset[i].cells[0] << " | " << dataset[i].cells[1] << " | " << dataset[i].cells[2] << "\n";
                foundAny = true;
            }
        }
    }
    if (!foundAny) std::cout << "No matching active records found.\n";
}

// Updates an active student field (except field 0 which is Roll No)
bool updateStudent(const std::string& roll, int fieldIndex, const std::string& newValue) {
    if (fieldIndex == 0) return false; // Rule: baseline configuration prevents updating structural IDs

    DataRow dataset[MAX_ROWS];
    int count = readTXT("students.txt", dataset);

    int index = findRow(dataset, count, 0, roll);
    if (index == -1 || dataset[index].cells[4] != "active") return false;

    // Perform specific rules validation if name is updating
    if (fieldIndex == 1 && !validateName(newValue)) return false;

    dataset[index].cells[fieldIndex] = newValue;
    return writeTXT("students.txt", dataset, count, "roll,name,dept,cgpa,status");
}

// Executes soft delete by switching status to inactive instead of deletion
bool deleteStudent(const std::string& roll) {
    return updateStudent(roll, 4, "inactive");
}
//hy