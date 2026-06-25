#include "attendance.h"
#include <iostream>

// Helper structure to keep backup states inside memory safely without standard container helpers
static DataRow attendanceBackup[MAX_ROWS];
static int backupRowCount = 0;
static bool hasBackup = false;

// Iterates over enrolled students, registers states, and keeps a state backup
void markAttendance(const std::string& courseCode, const std::string& date) {
    // 1. Create a complete file backup snapshot before editing
    DataRow currentLogs[MAX_ROWS];
    int currentCount = readTXT("attendance_log.txt", currentLogs);
    
    backupRowCount = 0;
    for (int i = 0; i < currentCount; ++i) {
        attendanceBackup[backupRowCount++] = currentLogs[i];
    }
    hasBackup = true;

    // 2. Fetch all currently active enrollments for this specific course
    DataRow enrollments[MAX_ROWS];
    int enrollCount = readTXT("enrollments.txt", enrollments);

    std::cout << "\n--- Marking Attendance for " << courseCode << " on " << date << " ---\n";
    std::cout << "Enter P for Present, A for Absent, L for Late\n";

    for (int i = 0; i < enrollCount; ++i) {
        if (enrollments[i].cells[1] == courseCode && enrollments[i].cells[3] == "enrolled") {
            std::string studentRoll = enrollments[i].cells[0];
            std::string status = "";
            
            while (true) {
                std::cout << "Roll No " << studentRoll << ": ";
                std::cin >> status;
                // Force basic uppercase correction tracking
                if (status == "p") status = "P";
                if (status == "a") status = "A";
                if (status == "l") status = "L";

                if (status == "P" || status == "A" || status == "L") {
                    break;
                }
                std::cout << "Invalid input. Please enter P, A, or L.\n";
            }

            // Map standard layout schema structure: roll, course_code, date, status
            DataRow newLog;
            newLog.cells[0] = studentRoll;
            newLog.cells[1] = courseCode;
            newLog.cells[2] = date;
            newLog.cells[3] = status;
            newLog.colCount = 4;

            appendTXT("attendance_log.txt", newLog);
        }
    }
    std::cout << "Attendance sheet recorded successfully.\n";
}

// Computes structural (Present + 0.5 * Late) percentages mathematically 
double getAttendancePct(const std::string& roll, const std::string& courseCode) {
    DataRow logs[MAX_ROWS];
    int count = readTXT("attendance_log.txt", logs);

    double presentCount = 0.0;
    double lateCount = 0.0;
    int totalSessions = 0;

    for (int i = 0; i < count; ++i) {
        if (logs[i].cells[0] == roll && logs[i].cells[1] == courseCode) {
            totalSessions++;
            if (logs[i].cells[3] == "P") {
                presentCount += 1.0;
            } else if (logs[i].cells[3] == "L") {
                lateCount += 1.0;
            }
        }
    }

    if (totalSessions == 0) return 100.0; // Default threshold if no sessions have run yet

    return ((presentCount + (0.5 * lateCount)) / totalSessions) * 100.0;
}

// Flags and groups short attendance tracks breaching university criteria
void getShortageList(const std::string& courseCode) {
    DataRow enrollments[MAX_ROWS];
    int enrollCount = readTXT("enrollments.txt", enrollments);
    
    std::cout << "\n--- Attendance Shortage List (< 75%) for " << courseCode << " ---\n";
    bool foundAny = false;

    for (int i = 0; i < enrollCount; ++i) {
        if (enrollments[i].cells[1] == courseCode && enrollments[i].cells[3] == "enrolled") {
            std::string roll = enrollments[i].cells[0];
            double pct = getAttendancePct(roll, courseCode);
            
            if (pct < 75.0) {
                std::cout << "Roll No: " << roll << " | Attendance: " << pct << "%\n";
                foundAny = true;
            }
        }
    }
    if (!foundAny) {
        std::cout << "All enrolled students clear current attendance targets!\n";
    }
}

// Restores the previous operational state completely using manual file system snapshots
bool undoLastSession() {
    if (!hasBackup) {
        std::cerr << "Undo action failed: No active pre-session snapshot states saved.\n";
        return false;
    }

    bool success = writeTXT("attendance_log.txt", attendanceBackup, backupRowCount, "roll,course_code,date,status");
    if (success) {
        hasBackup = false; // Consume backup slot safely
        std::cout << "Rollback successful. Last attendance session removed.\n";
    }
    return success;
}

// Displays structural console tables mapping operational student sessions
void printDailySheet(const std::string& courseCode, const std::string& date) {
    DataRow logs[MAX_ROWS];
    int count = readTXT("attendance_log.txt", logs);

    std::cout << "\n--- Daily Attendance Sheet: " << courseCode << " (" << date << ") ---\n";
    std::cout << "Roll Number   | Status\n";
    std::cout << "-----------------------\n";
    
    for (int i = 0; i < count; ++i) {
        if (logs[i].cells[1] == courseCode && logs[i].cells[2] == date) {
            std::cout << logs[i].cells[0] << "   |   " << logs[i].cells[3] << "\n";
        }
    }
    //hy
}