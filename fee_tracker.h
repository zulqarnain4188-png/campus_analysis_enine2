#ifndef FEE_TRACKER_H
#define FEE_TRACKER_H

#include <string>
#include "filehandler.h"

// Structure to match financial tracking records
struct FeeRecord {
    std::string rollNumber;
    std::string semester;
    double amountDue;
    double amountPaid;
    std::string dueDate;
    std::string paidDate;
};

// Required function signatures from specification
bool recordPayment(const std::string& roll, const std::string& semester, double paidAmount, const std::string& paidDate);
double computeLateFine(double amountDue, const std::string& dueDate, const std::string& paidDate);
int daysBetween(const std::string& date1, const std::string& date2);
void generateReceipt(const std::string& roll, const std::string& semester);
void getDefaulters();

// Helper date format validator
bool validateDateFormat(const std::string& dateStr);

#endif