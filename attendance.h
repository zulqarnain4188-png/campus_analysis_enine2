#ifndef ATTENDANCE_H
#define ATTENDANCE_H

#include <string>
#include "filehandler.h"

// Required function signatures from specification
void markAttendance(const std::string& courseCode, const std::string& date);
double getAttendancePct(const std::string& roll, const std::string& courseCode);
void getShortageList(const std::string& courseCode);
bool undoLastSession();
void printDailySheet(const std::string& courseCode, const std::string& date);

#endif