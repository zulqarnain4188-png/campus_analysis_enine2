#include <iostream>
#include <string>
#include "filehandler.h"
#include "student_ops.h"
#include "course_ops.h"
#include "attendance.h"
#include "grades.h"
#include "fee_tracker.h"
#include "reports.h"
//This is the main function

// Helper function to show structural console interface sub-headers
void displayHeader(const std::string& title) {
    std::cout << "\n=========================================\n";
    std::cout << "      " << title << "      \n";
    std::cout << "=========================================\n";
}

int main() {
    int mainChoice = 0;

    while (true) {
        std::cout << "\n=========================================\n";
        std::cout << "     THE CAMPUS ANALYTICS ENGINE         \n";
        std::cout << "=========================================\n";
        std::cout << "1. Student Profile Management\n";
        std::cout << "2. Course Enrollment System\n";
        std::cout << "3. Attendance Management Matrix\n";
        std::cout << "4. Examination Grading Registry\n";
        std::cout << "5. Student Financial Ledger\n";
        std::cout << "6. Institutional Report Center\n";
        std::cout << "7. Exit Application\n";
        std::cout << "=========================================\n";
        std::cout << "Enter your system choice: ";
        
        if (!(std::cin >> mainChoice)) {
            std::cin.clear();
            std::string discard;
            std::getline(std::cin, discard);
            std::cout << "Invalid numeric input format.\n";
            continue;
        }

        if (mainChoice == 7) {
            std::cout << "\nShutting down Campus Analytics Engine. Safe travels!\n";
            break;
        }

        switch (mainChoice) {
            case 1: { // Student Profile Panel Loop
                int subChoice = 0;
                while (true) {
                    displayHeader("STUDENT PROFILE MANAGEMENT");
                    std::cout << "1. Add New Student Profile\n";
                    std::cout << "2. Search Student Profile by Roll\n";
                    std::cout << "3. Filter Profiles by Prefix Name\n";
                    std::cout << "4. Soft-Delete Student Profile\n";
                    std::cout << "5. Return to Main Dashboard\n";
                    std::cout << "-----------------------------------------\n";
                    std::cout << "Select Operation: ";
                    std::cin >> subChoice;
                    
                    if (subChoice == 5) break;

                    if (subChoice == 1) {
                        std::string roll, name, dept, cgpa;
                        std::cout << "Enter Roll Number (Format: BSAI-YY-XXX): ";
                        std::cin >> roll;
                        std::cout << "Enter Full Name (No Numbers Allowed): ";
                        std::cin.ignore();
                        std::getline(std::cin, name);
                        std::cout << "Enter Department Name: ";
                        std::cin >> dept;
                        std::cout << "Enter Entry CGPA: ";
                        std::cin >> cgpa;

                        if (addStudent(roll, name, dept, cgpa)) {
                            std::cout << "Student profile written to disk successfully.\n";
                        }
                    } else if (subChoice == 2) {
                        std::string roll;
                        std::cout << "Enter Target Roll Number: ";
                        std::cin >> roll;
                        searchByRoll(roll);
                    } else if (subChoice == 3) {
                        std::string prefix;
                        std::cout << "Type Search-As-You-Type Name Prefix: ";
                        std::cin >> prefix;
                        searchByName(prefix);
                    } else if (subChoice == 4) {
                        std::string roll;
                        std::cout << "Enter Profile Roll to Delete (Soft-Delete): ";
                        std::cin >> roll;
                        if (deleteStudent(roll)) {
                            std::cout << "Profile changed to inactive status successfully.\n";
                        } else {
                            std::cout << "Failed to locate active profile.\n";
                        }
                    }
                }
                break;
            }

            case 2: { // Course Enrollment Interface
                int subChoice = 0;
                while (true) {
                    displayHeader("COURSE ENROLLMENT SYSTEM");
                    std::cout << "1. Register Student for Course\n";
                    std::cout << "2. Drop Active Student Course Track\n";
                    std::cout << "3. List Course Active Enrollments\n";
                    std::cout << "4. Return to Main Dashboard\n";
                    std::cout << "-----------------------------------------\n";
                    std::cout << "Select Operation: ";
                    std::cin >> subChoice;

                    if (subChoice == 4) break;

                    if (subChoice == 1) {
                        std::string roll, course, sem;
                        std::cout << "Enter Student Roll Number: ";
                        std::cin >> roll;
                        std::cout << "Enter Course Code: ";
                        std::cin >> course;
                        std::cout << "Enter Semester Label: ";
                        std::cin >> sem;

                        EnrollResult res = enrollStudent(roll, course, sem);
                        if (res == ENROLL_SUCCESS) std::cout << "Enrollment successfully authorized.\n";
                        else if (res == STUDENT_INACTIVE) std::cout << "Rejected: Profile inactive or does not exist.\n";
                        else if (res == COURSE_NOT_FOUND) std::cout << "Rejected: Course code completely absent from catalog.\n";
                        else if (res == NO_SEATS) std::cout << "Rejected: Course capacity thresholds completely full.\n";
                        else if (res == ALREADY_ENROLLED) std::cout << "Rejected: Enrollment entry duplicate path discovered.\n";
                        else if (res == CREDIT_LIMIT_EXCEEDED) std::cout << "Rejected: Absolute semester workload will cross 21 hrs.\n";
                        else if (res == PREREQ_FAILED) std::cout << "Rejected: Prerequisite validation constraints failed (F Grade).\n";
                    } else if (subChoice == 2) {
                        std::string roll, course, sem;
                        std::cout << "Enter Student Roll Number: ";
                        std::cin >> roll;
                        std::cout << "Enter Course Code: ";
                        std::cin >> course;
                        std::cout << "Enter Semester Label: ";
                        std::cin >> sem;

                        if (dropCourse(roll, course, sem)) {
                            std::cout << "Course entry status rewritten as dropped.\n";
                        }
                    } else if (subChoice == 3) {
                        std::string course;
                        std::cout << "Enter Course Code: ";
                        std::cin >> course;
                        listenEnrolledStudents(course);
                    }
                }
                break;
            }

            case 3: { // Attendance Panel
                int subChoice = 0;
                while (true) {
                    displayHeader("ATTENDANCE MANAGEMENT MATRIX");
                    std::cout << "1. Run Attendance Session Logging\n";
                    std::cout << "2. Check Course Shortage List (<75%)\n";
                    std::cout << "3. Rollback (Undo) Last Recorded Session\n";
                    std::cout << "4. Return to Main Dashboard\n";
                    std::cout << "-----------------------------------------\n";
                    std::cout << "Select Operation: ";
                    std::cin >> subChoice;

                    if (subChoice == 4) break;

                    if (subChoice == 1) {
                        std::string course, date;
                        std::cout << "Enter Course Code: ";
                        std::cin >> course;
                        std::cout << "Enter Date (DD-MM-YYYY): ";
                        std::cin >> date;
                        markAttendance(course, date);
                    } else if (subChoice == 2) {
                        std::string course;
                        std::cout << "Enter Course Code: ";
                        std::cin >> course;
                        getShortageList(course);
                    } else if (subChoice == 3) {
                        undoLastSession();
                    }
                }
                break;
            }

            case 4: { // Examination Marks
                int subChoice = 0;
                while (true) {
                    displayHeader("EXAMINATION GRADING REGISTRY");
                    std::cout << "1. Input Student Course Assessment Marks\n";
                    std::cout << "2. Calculate Class Distribution States\n";
                    std::cout << "3. Return to Main Dashboard\n";
                    std::cout << "-----------------------------------------\n";
                    std::cout << "Select Operation: ";
                    std::cin >> subChoice;

                    if (subChoice == 3) break;

                    if (subChoice == 1) {
                        std::string roll, course;
                        double quizzes[5];
                        double asgn, mid, finalEx;

                        std::cout << "Enter Student Roll Number: ";
                        std::cin >> roll;
                        std::cout << "Enter Course Code: ";
                        std::cin >> course;
                        
                        std::cout << "Enter 5 Quiz Scores consecutively:\n";
                        for (int i = 0; i < 5; ++i) {
                            std::cout << "Quiz " << (i + 1) << ": ";
                            std::cin >> quizzes[i];
                        }
                        std::cout << "Enter Assignment Aggregated Mark: ";
                        std::cin >> asgn;
                        std::cout << "Enter Midterm Examination Score: ";
                        std::cin >> mid;
                        std::cout << "Enter Final Examination Score: ";
                        std::cin >> finalEx;

                        if (enterMarks(roll, course, quizzes, asgn, mid, finalEx)) {
                            std::cout << "Grades committed to data registry.\n";
                        }
                    } else if (subChoice == 2) {
                        std::string course;
                        std::cout << "Enter Course Code: ";
                        std::cin >> course;
                        Stats classData = computeClassState(course);
                        std::cout << "\nClass Statistical Distribution for " << course << ":\n";
                        std::cout << "- Highest Total Score: " << classData.highest << "\n";
                        std::cout << "- Lowest Total Score:  " << classData.lowest << "\n";
                        std::cout << "- Mean (Class Average): " << classData.mean << "\n";
                        std::cout << "- Median Score Mark:   " << classData.median << "\n";
                    }
                }
                break;
            }

            case 5: { // Financial Tracking Menu
                int subChoice = 0;
                while (true) {
                    displayHeader("STUDENT FINANCIAL LEDGER");
                    std::cout << "1. Record New Fee Payment Transaction\n";
                    std::cout << "2. Generate Fee Assessment Invoice Receipt\n";
                    std::cout << "3. Display Institution Financial Defaulters List\n";
                    std::cout << "4. Return to Main Dashboard\n";
                    std::cout << "-----------------------------------------\n";
                    std::cout << "Select Operation: ";
                    std::cin >> subChoice;

                    if (subChoice == 4) break;

                    if (subChoice == 1) {
                        std::string roll, sem, date;
                        double amt;
                        std::cout << "Enter Student Roll Number: ";
                        std::cin >> roll;
                        std::cout << "Enter Semester Code: ";
                        std::cin >> sem;
                        std::cout << "Enter Paid Amount Vector: Rs. ";
                        std::cin >> amt;
                        std::cout << "Enter Payment Date (DD-MM-YYYY): ";
                        std::cin >> date;

                        if (recordPayment(roll, sem, amt, date)) {
                            std::cout << "Financial ledger record re-balanced successfully.\n";
                        } else {
                            std::cout << "Record adjustment execution failed (Check Date/Roll formatting).\n";
                        }
                    } else if (subChoice == 2) {
                        std::string roll, sem;
                        std::cout << "Enter Student Roll Number: ";
                        std::cin >> roll;
                        std::cout << "Enter Semester Code: ";
                        std::cin >> sem;
                        generateReceipt(roll, sem);
                    } else if (subChoice == 3) {
                        getDefaulters();
                    }
                }
                break;
            }

            case 6: { // Institutional Reporting Center Menu
                int subChoice = 0;
                while (true) {
                    displayHeader("INSTITUTIONAL REPORT CENTER");
                    std::cout << "1. Run Department Merit List Matrix\n";
                    std::cout << "2. Display Full Academic Transcript Summary\n";
                    std::cout << "3. Return to Main Dashboard\n";
                    std::cout << "-----------------------------------------\n";
                    std::cout << "Select Operation: ";
                    std::cin >> subChoice;

                    if (subChoice == 3) break;

                    if (subChoice == 1) {
                        std::string dept;
                        std::cout << "Enter Target Department Code: ";
                        std::cin >> dept;
                        generateMeritList(dept);
                    } else if (subChoice == 2) {
                        std::string roll;
                        std::cout << "Enter Student Roll Number: ";
                        std::cin >> roll;
                        printPerformanceSummary(roll);
                    }
                }
                break;
            }

            default:
                std::cout << "System Choice Out of Range. Re-Enter from 1-7.\n";
        }
    }
    return 0;
}