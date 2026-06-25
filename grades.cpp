#include "grades.h"
#include "attendance.h"
#include <iostream>
#include <sstream>

static int localStringToInt(const std::string& str) {
    int res = 0;
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] >= '0' && str[i] <= '9') res = res * 10 + (str[i] - '0');
    }
    return res;
}

static double manualToDouble(const std::string& str) {
    double res = 0.0;
    double factor = 1.0;
    bool decimalFound = false;
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == '.') {
            decimalFound = true;
            continue;
        }
        if (str[i] >= '0' && str[i] <= '9') {
            if (decimalFound) {
                factor *= 0.1;
                res += (str[i] - '0') * factor;
            } else {
                res = res * 10.0 + (str[i] - '0');
            }
        }
    }
    return res;
}

static std::string doubleToString(double num) {
    std::stringstream ss;
    ss << num;
    return ss.str();
}

double bestThreeOfFive(const double quizzes[5]) {
    double total = 0.0;
    double min1 = 999.0, min2 = 999.0;
    int idx1 = -1;

    for (int i = 0; i < 5; ++i) {
        total += quizzes[i];
        if (quizzes[i] < min1) {
            min1 = quizzes[i];
            idx1 = i;
        }
    }

    for (int i = 0; i < 5; ++i) {
        if (i == idx1) continue;
        if (quizzes[i] < min2) {
            min2 = quizzes[i];
        }
    }

    return (total - min1 - min2) / 3.0;
}

double computeWeightedTotal(double quizAvg, double asgn, double mid, double finalExam) {
    return (quizAvg * 0.10) + (asgn * 0.10) + (mid * 0.30) + (finalExam * 0.50);
}

std::string getLetterGrade(double totalScore) {
    if (totalScore >= 85.0) return "A";
    if (totalScore >= 80.0) return "B+";
    if (totalScore >= 70.0) return "B";
    if (totalScore >= 65.0) return "C+";
    if (totalScore >= 60.0) return "C";
    if (totalScore >= 50.0) return "D";
    return "F";
}

bool applyAttendancePenalty(const std::string& roll, const std::string& courseCode, std::string& finalLetter) {
    double attPct = getAttendancePct(roll, courseCode);
    if (attPct < 75.0) {
        finalLetter = "F";
        return true;
    }
    return false;
}

bool enterMarks(const std::string& roll, const std::string& courseCode, const double quizzes[5], double assignment, double mid, double finalExam) {
    double quizAvg = bestThreeOfFive(quizzes);
    double totalScore = computeWeightedTotal(quizAvg, assignment, mid, finalExam);
    std::string letterGrade = getLetterGrade(totalScore);

    applyAttendancePenalty(roll, courseCode, letterGrade);

    DataRow dataset[MAX_ROWS];
    int count = readTXT("grades.txt", dataset);

    int index = -1;
    for (int i = 0; i < count; ++i) {
        if (dataset[i].cells[0] == roll && dataset[i].cells[1] == courseCode) {
            index = i;
            break;
        }
    }

    if (index != -1) {
        dataset[index].cells[2] = doubleToString(totalScore);
        dataset[index].cells[3] = letterGrade;
        return writeTXT("grades.txt", dataset, count, "roll,course_code,total_score,letter_grade");
    } else {
        DataRow newGrade;
        newGrade.cells[0] = roll;
        newGrade.cells[1] = courseCode;
        newGrade.cells[2] = doubleToString(totalScore);
        newGrade.cells[3] = letterGrade;
        newGrade.colCount = 4;
        return appendTXT("grades.txt", newGrade);
    }
}

double computeGPA(const std::string& roll, const std::string& semester) {
    DataRow grades[MAX_ROWS];
    DataRow enrollments[MAX_ROWS];
    DataRow courses[MAX_ROWS];

    int gradeCount = readTXT("grades.txt", grades);
    int enrollCount = readTXT("enrollments.txt", enrollments);
    int courseCount = readTXT("courses.txt", courses);

    double totalPoints = 0.0;
    int totalCredits = 0;

    for (int i = 0; i < enrollCount; ++i) {
        if (enrollments[i].cells[0] == roll && enrollments[i].cells[2] == semester && enrollments[i].cells[3] == "enrolled") {
            std::string cCode = enrollments[i].cells[1];
            
            int creditHours = 0;
            for (int k = 0; k < courseCount; ++k) {
                if (courses[k].cells[0] == cCode) {
                    creditHours = localStringToInt(courses[k].cells[2]);
                    break;
                }
            }

            for (int j = 0; j < gradeCount; ++j) {
                if (grades[j].cells[0] == roll && grades[j].cells[1] == cCode) {
                    std::string grade = grades[j].cells[3];
                    double points = 0.0;
                    if (grade == "A") points = 4.0;
                    else if (grade == "B+") points = 3.5;
                    else if (grade == "B") points = 3.0;
                    else if (grade == "C+") points = 2.5;
                    else if (grade == "C") points = 2.0;
                    else if (grade == "D") points = 1.0;
                    else points = 0.0;

                    totalPoints += (points * creditHours);
                    totalCredits += creditHours;
                    break;
                }
            }
        }
    }
    if (totalCredits == 0) return 0.0;
    return totalPoints / totalCredits;
}

Stats computeClassState(const std::string& courseCode) {
    DataRow grades[MAX_ROWS];
    int count = readTXT("grades.txt", grades);

    double scores[MAX_ROWS];
    int totalScores = 0;
    double sum = 0.0;
    double highest = 0.0;
    double lowest = 100.0;

    for (int i = 0; i < count; ++i) {
        if (grades[i].cells[1] == courseCode) {
            double currentScore = manualToDouble(grades[i].cells[2]);
            scores[totalScores++] = currentScore;
            sum += currentScore;
            if (currentScore > highest) highest = currentScore;
            if (currentScore < lowest) lowest = currentScore;
        }
    }

    Stats classStats = {0.0, 0.0, 0.0, 0.0};
    if (totalScores == 0) return classStats;

    classStats.highest = highest;
    classStats.lowest = lowest;
    classStats.mean = sum / totalScores;

    for (int i = 0; i < totalScores - 1; ++i) {
        for (int j = 0; j < totalScores - i - 1; ++j) {
            if (scores[j] > scores[j+1]) {
                double temp = scores[j];
                scores[j] = scores[j+1];
                scores[j+1] = temp;
            }
        }
    }

    if (totalScores % 2 == 0) {
        classStats.median = (scores[totalScores / 2 - 1] + scores[totalScores / 2]) / 2.0;
    } else {
        classStats.median = scores[totalScores / 2];
    }

    return classStats;
}