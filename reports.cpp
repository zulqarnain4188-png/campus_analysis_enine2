#include "reports.h"
#include "grades.h"
#include "attendance.h"
#include <iostream>
#include <fstream>
#include <iomanip>
//Reports

// Generates a clean department merit list using a custom manual bubble sort loop
void generateMeritList(const std::string& dept) {
    DataRow dataset[MAX_ROWS];
    int count = readTXT("students.txt", dataset);

    DataRow filteredRows[MAX_ROWS];
    double cgpas[MAX_ROWS];
    int filteredCount = 0;

    // Filter by department and active status
    for (int i = 0; i < count; ++i) {
        if (dataset[i].cells[2] == dept && dataset[i].cells[4] == "active") {
            filteredRows[filteredCount] = dataset[i];
            cgpas[filteredCount] = std::stod(dataset[i].cells[3]);
            filteredCount++;
        }
    }

    if (filteredCount == 0) {
        std::cout << "No active student records found for department: " << dept << "\n";
        return;
    }

    // Required structural Bubble Sort to arrange CGPAs in descending order
    for (int i = 0; i < filteredCount - 1; ++i) {
        for (int j = 0; j < filteredCount - i - 1; ++j) {
            if (cgpas[j] < cgpas[j + 1]) {
                // Swap CGPA tracking elements
                double tempGpa = cgpas[j];
                cgpas[j] = cgpas[j + 1];
                cgpas[j + 1] = tempGpa;

                // Swap accompanying structure rows
                DataRow tempRow = filteredRows[j];
                filteredRows[j] = filteredRows[j + 1];
                filteredRows[j + 1] = tempRow;
            }
        }
    }

    std::cout << "\n=============================================\n";
    std::cout << "     MERIT LIST FOR DEPARTMENT: " << dept << "     \n";
    std::cout << "=============================================\n";
    std::cout << "Rank | Roll Number   | Student Name         | CGPA\n";
    std::cout << "---------------------------------------------\n";
    for (int i = 0; i < filteredCount; ++i) {
        std::cout << std::left << std::setw(4) << (i + 1) << " | "
                  << std::setw(13) << filteredRows[i].cells[0] << " | "
                  << std::setw(20) << filteredRows[i].cells[1] << " | "
                  << std::fixed << std::setprecision(2) << cgpas[i] << "\n";
    }
    std::cout << "=============================================\n";
}

// Combines enrollment codes, grades, and attendance rates to render a full transcript summary
void printPerformanceSummary(const std::string& roll) {
    DataRow students[MAX_ROWS];
    int sCount = readTXT("students.txt", students);
    int sIdx = findRow(students, sCount, 0, roll);

    if (sIdx == -1 || students[sIdx].cells[4] != "active") {
        std::cout << "Active student profile record not found.\n";
        return;
    }

    DataRow enrollments[MAX_ROWS];
    int eCount = readTXT("enrollments.txt", enrollments);
    DataRow grades[MAX_ROWS];
    int gCount = readTXT("grades.txt", grades);

    std::cout << "\n=======================================================\n";
    std::cout << "             ACADEMIC PERFORMANCE SUMMARY              \n";
    std::cout << "=======================================================\n";
    std::cout << "Student Name: " << students[sIdx].cells[1] << "\n";
    std::cout << "Roll Number:  " << students[sIdx].cells[0] << "\n";
    std::cout << "Department:   " << students[sIdx].cells[2] << "\n";
    std::cout << "Baseline CGPA: " << students[sIdx].cells[3] << "\n";
    std::cout << "-------------------------------------------------------\n";
    std::cout << "Course Code | Final Score | Letter Grade | Attendance %\n";
    std::cout << "-------------------------------------------------------\n";

    bool registeredAny = false;
    for (int i = 0; i < eCount; ++i) {
        if (enrollments[i].cells[0] == roll && enrollments[i].cells[3] == "enrolled") {
            std::string cCode = enrollments[i].cells[1];
            std::string score = "N/A";
            std::string letter = "N/A";

            // Cross-reference current grades matrix row values
            for (int j = 0; j < gCount; ++j) {
                if (grades[j].cells[0] == roll && grades[j].cells[1] == cCode) {
                    score = grades[j].cells[2];
                    letter = grades[j].cells[3];
                    break;
                }
            }

            double attendancePct = getAttendancePct(roll, cCode);
            
            std::cout << std::left << std::setw(11) << cCode << " | "
                      << std::setw(11) << score << " | "
                      << std::setw(12) << letter << " | "
                      << std::fixed << std::setprecision(1) << attendancePct << "%\n";
            registeredAny = true;
        }
    }

    if (!registeredAny) {
        std::cout << "No active course enrollment trails flagged for this profile.\n";
    }
    std::cout << "=======================================================\n";
}

// Exports processed content snapshots directly into a plain text analytical report file
bool exportReport(const std::string& reportFilename, const std::string& content) {
    std::ofstream outFile(reportFilename);
    if (!outFile.is_open()) {
        return false;
    }
    outFile << content;
    outFile.close();
    return true;
}