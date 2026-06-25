#ifndef STUDENT_OPS_H
#define STUDENT_OPS_H

#include <string>
#include "filehandler.h"

// Define a clean structural representation (No OOP classes used!)
struct Student {
    std::string rollNumber;
    std::string name;
    std::string department;
    std::string cgpa;
    std::string status; // "active" or "inactive" (soft delete)
};

// Required function signatures from specification
bool addStudent(const std::string& roll, const std::string& name, const std::string& dept, const std::string& cgpa);
void searchByRoll(const std::string& roll);
void searchByName(const std::string& nameSubstr);
bool updateStudent(const std::string& roll, int fieldIndex, const std::string& newValue);
bool deleteStudent(const std::string& roll);

// Helper validation functions
bool validateRollFormat(const std::string& roll);
bool validateName(const std::string& name);

#endif