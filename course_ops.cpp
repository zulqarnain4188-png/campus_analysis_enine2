#include "course_ops.h"
#include <iostream>

// Helper to safely convert string numbers to integer without forbidden libraries
int manualToInt(const std::string& str) {
    int res = 0;
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] >= '0' && str[i] <= '9') {
            res = res * 10 + (str[i] - '0');
        }
    }
    return res;
}

// Sums credit hours using a mandatory nested structural loop across files
int getCreditLoad(const std::string& roll, const std::string& semester) {
    DataRow enrollments[MAX_ROWS];
    DataRow courses[MAX_ROWS];
    
    int enrollCount = readTXT("enrollments.txt", enrollments);
    int courseCount = readTXT("courses.txt", courses);
    int totalCredits = 0;

    for (int i = 0; i < enrollCount; ++i) {
        // Look up active enrollments for this specific student and semester
        if (enrollments[i].cells[0] == roll && 
            enrollments[i].cells[2] == semester && 
            enrollments[i].cells[3] == "enrolled") {
            
            std::string currentCourse = enrollments[i].cells[1];
            
            // Nested loop to look up matching course credit weight
            for (int j = 0; j < courseCount; ++j) {
                if (courses[j].cells[0] == currentCourse) {
                    totalCredits += manualToInt(courses[j].cells[2]);
                    break;
                }
            }
        }
    }
    return totalCredits;
}

// Looks up pre-req strings and checks historical grade outputs manually
bool checkPrerequisite(const std::string& roll, const std::string& courseCode) {
    DataRow courses[MAX_ROWS];
    int courseCount = readTXT("courses.txt", courses);
    
    int courseIdx = findRow(courses, courseCount, 0, courseCode);
    if (courseIdx == -1) return false;

    std::string prereq = courses[courseIdx].cells[4];
    if (prereq == "NONE") return true;

    // Read grades tracking file to ensure prerequisite was passed (Not an F grade)
    DataRow grades[MAX_ROWS];
    int gradeCount = readTXT("grades.txt", grades);

    for (int i = 0; i < gradeCount; ++i) {
        // Matching rules: student roll, prereq course, and letter grade != F
        if (grades[i].cells[0] == roll && grades[i].cells[1] == prereq) {
            std::string letterGrade = grades[i].cells[4]; // assuming field 4 holds letter grade
            if (letterGrade != "F") {
                return true;
            }
        }
    }
    return false;
}

// Executes enrollment workflow verifying all constraints safely
EnrollResult enrollStudent(const std::string& roll, const std::string& courseCode, const std::string& semester) {
    DataRow students[MAX_ROWS];
    int studentCount = readTXT("students.txt", students);
    int studentIdx = findRow(students, studentCount, 0, roll);
    
    if (studentIdx == -1 || students[studentIdx].cells[4] != "active") {
        return STUDENT_INACTIVE;
    }

    DataRow courses[MAX_ROWS];
    int courseCount = readTXT("courses.txt", courses);
    int courseIdx = findRow(courses, courseCount, 0, courseCode);
    
    if (courseIdx == -1) {
        return COURSE_NOT_FOUND;
    }

    // Check capacity thresholds
    int seats = manualToInt(courses[courseIdx].cells[3]);
    DataRow enrollments[MAX_ROWS];
    int enrollCount = readTXT("enrollments.txt", enrollments);
    
    int occupiedSeats = 0;
    for (int i = 0; i < enrollCount; ++i) {
        if (enrollments[i].cells[1] == courseCode && enrollments[i].cells[3] == "enrolled") {
            occupiedSeats++;
        }
        // Duplicate check
        if (enrollments[i].cells[0] == roll && enrollments[i].cells[1] == courseCode && enrollments[i].cells[3] == "enrolled") {
            return ALREADY_ENROLLED;
        }
    }

    if (occupiedSeats >= seats) return NO_SEATS;
    if (!checkPrerequisite(roll, courseCode)) return PREREQ_FAILED;

    // Confirm that credit cap doesn't breach 21 hours limit
    int currentCourseCredits = manualToInt(courses[courseIdx].cells[2]);
    if (getCreditLoad(roll, semester) + currentCourseCredits > 21) {
        return CREDIT_LIMIT_EXCEEDED;
    }

    // Create entry mapping: roll, course_code, semester, status
    DataRow newEnroll;
    newEnroll.cells[0] = roll;
    newEnroll.cells[1] = courseCode;
    newEnroll.cells[2] = semester;
    newEnroll.cells[3] = "enrolled";
    newEnroll.colCount = 4;

    appendTXT("enrollments.txt", newEnroll);
    return ENROLL_SUCCESS;
}

// Drops a course only if zero attendance logging trails exist
bool dropCourse(const std::string& roll, const std::string& courseCode, const std::string& semester) {
    DataRow attendance[MAX_ROWS];
    int attCount = readTXT("attendance_log.txt", attendance);

    // Enforce Rule: Drop only permitted if no attendance rows exist
    for (int i = 0; i < attCount; ++i) {
        if (attendance[i].cells[0] == roll && attendance[i].cells[1] == courseCode) {
            std::cerr << "Cannot drop course: Attendance logs exist for this record.\n";
            return false;
        }
    }

    DataRow enrollments[MAX_ROWS];
    int enrollCount = readTXT("enrollments.txt", enrollments);

    for (int i = 0; i < enrollCount; ++i) {
        if (enrollments[i].cells[0] == roll && 
            enrollments[i].cells[1] == courseCode && 
            enrollments[i].cells[2] == semester && 
            enrollments[i].cells[3] == "enrolled") {
            
            enrollments[i].cells[3] = "dropped";
            return writeTXT("enrollments.txt", enrollments, enrollCount, "roll,course_code,semester,status");
        }
    }
    return false;
}

// Prints out active student roll configurations inside target course
void listenEnrolledStudents(const std::string& courseCode) {
    DataRow enrollments[MAX_ROWS];
    int enrollCount = readTXT("enrollments.txt", enrollments);
    
    std::cout << "\nStudents Enrolled in " << courseCode << ":\n";
    for (int i = 0; i < enrollCount; ++i) {
        if (enrollments[i].cells[1] == courseCode && enrollments[i].cells[3] == "enrolled") {
            std::cout << "- " << enrollments[i].cells[0] << "\n";
        }
        //also hy
    }
}