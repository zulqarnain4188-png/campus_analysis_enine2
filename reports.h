#ifndef REPORTS_H
#define REPORTS_H

#include <string>
#include "filehandler.h"

// Required function signatures from specification
void generateMeritList(const std::string& dept);
void printPerformanceSummary(const std::string& roll);
bool exportReport(const std::string& reportFilename, const std::string& content);

#endif