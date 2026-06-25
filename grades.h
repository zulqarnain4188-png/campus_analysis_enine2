#ifndef GRADES_H
#define GRADES_H

#include <string>
#include "filehandler.h"

// Struct for internal grade statistics tracking
struct Stats {
    double highest;
    double lowest;
    double mean;
    double median;
};

// Required function signatures from specification
bool enterMarks(const std::string& roll, const std::string& courseCode, const double quizzes[5], double assignment, double mid, double finalExam);
double bestThreeOfFive(const double quizzes[5]);
double computeWeightedTotal(double quizAvg, double asgn, double mid, double finalExam);
std::string getLetterGrade(double totalScore);
double computeGPA(const std::string& roll, const std::string& semester);
Stats computeClassState(const std::string& courseCode);
bool applyAttendancePenalty(const std::string& roll, const std::string& courseCode, std::string& finalLetter);

#endif