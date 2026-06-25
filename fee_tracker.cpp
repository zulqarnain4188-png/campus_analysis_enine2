#include "fee_tracker.h"
#include <iostream>
#include <iomanip>
//feetracker

// Validates the format: DD-MM-YYYY using zero external libraries
bool validateDateFormat(const std::string& dateStr) {
    if (dateStr.length() != 10) return false;
    if (dateStr[2] != '-' || dateStr[5] != '-') return false;

    for (int i = 0; i < 10; ++i) {
        if (i == 2 || i == 5) continue;
        if (dateStr[i] < '0' || dateStr[i] > '9') return false;
    }
    return true;
}

// Counts absolute cumulative days elapsed since a fixed point without using <ctime>
int daysBetween(const std::string& date1, const std::string& date2) {
    // Parse Date 1 (DD-MM-YYYY)
    int d1 = (date1[0]-'0')*10 + (date1[1]-'0');
    int m1 = (date1[3]-'0')*10 + (date1[4]-'0');
    int y1 = (date1[6]-'0')*1000 + (date1[7]-'0')*100 + (date1[8]-'0')*10 + (date1[9]-'0');

    // Parse Date 2 (DD-MM-YYYY)
    int d2 = (date2[0]-'0')*10 + (date2[1]-'0');
    int m2 = (date2[3]-'0')*10 + (date2[4]-'0');
    int y2 = (date2[6]-'0')*1000 + (date2[7]-'0')*100 + (date2[8]-'0')*10 + (date2[9]-'0');

    // Non-leap year month tracker distribution
    int months[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    long int n1 = y1 * 365 + d1;
    for (int i = 0; i < m1; i++) n1 += months[i];
    // Account for past leap years
    n1 += (y1 / 4) - (y1 / 100) + (y1 / 400);

    long int n2 = y2 * 365 + d2;
    for (int i = 0; i < m2; i++) n2 += months[i];
    n2 += (y2 / 4) - (y2 / 100) + (y2 / 400);

    return (n2 - n1);
}

// Computes 2% fine compounds per completed week overdue
double computeLateFine(double amountDue, const std::string& dueDate, const std::string& paidDate) {
    int elapsedDays = daysBetween(dueDate, paidDate);
    if (elapsedDays <= 0) return 0.0;

    int weeksLate = elapsedDays / 7;
    if (weeksLate == 0 && elapsedDays > 0) weeksLate = 1; // Any partial day past count as week penalty layer
    
    return amountDue * 0.02 * weeksLate;
}

// Records tuition payments, checking formats and adjusting totals cleanly
bool recordPayment(const std::string& roll, const std::string& semester, double paidAmount, const std::string& paidDate) {
    if (!validateDateFormat(paidDate)) {
        std::cerr << "Invalid date format. Must be DD-MM-YYYY\n";
        return false;
    }

    DataRow dataset[MAX_ROWS];
    int count = readTXT("fees.txt", dataset);

    for (int i = 0; i < count; ++i) {
        if (dataset[i].cells[0] == roll && dataset[i].cells[1] == semester) {
            double due = std::stod(dataset[i].cells[2]);
            double currentPaid = std::stod(dataset[i].cells[3]);
            std::string dueDate = dataset[i].cells[4];

            double fine = computeLateFine(due, dueDate, paidDate);
            due += fine; // Fine is added directly to outstanding ledger debt

            currentPaid += paidAmount;
            dataset[i].cells[2] = std::to_string(due);
            dataset[i].cells[3] = std::to_string(currentPaid);
            dataset[i].cells[5] = paidDate; // Update paid_date field tracking column

            return writeTXT("fees.txt", dataset, count, "roll,semester,amount_due,amount_paid,due_date,paid_date");
        }
    }
    return false;
}

// Prints a tuition checkout receipt layout cleanly via iomanip tools
void generateReceipt(const std::string& roll, const std::string& semester) {
    DataRow dataset[MAX_ROWS];
    int count = readTXT("fees.txt", dataset);

    int idx = -1;
    for (int i = 0; i < count; ++i) {
        if (dataset[i].cells[0] == roll && dataset[i].cells[1] == semester) {
            idx = i;
            break;
        }
    }

    if (idx == -1) {
        std::cout << "No matching billing records exist for student.\n";
        return;
    }

    double due = std::stod(dataset[idx].cells[2]);
    double paid = std::stod(dataset[idx].cells[3]);
    double balance = due - paid;

    std::cout << "\n====================================\n";
    std::cout << "        FEES PAYMENT RECEIPT        \n";
    std::cout << "====================================\n";
    std::cout << "Roll Number: " << dataset[idx].cells[0] << "\n";
    std::cout << "Semester:    " << dataset[idx].cells[1] << "\n";
    std::cout << "------------------------------------\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Total Amount Due:  Rs. " << due << "\n";
    std::cout << "Total Amount Paid: Rs. " << paid << "\n";
    std::cout << "Outstanding Bal:   Rs. " << balance << "\n";
    std::cout << "Due Date Limit:    " << dataset[idx].cells[4] << "\n";
    std::cout << "Last Action Date:  " << dataset[idx].cells[5] << "\n";
    std::cout << "====================================\n";
}

// Lists outstanding defaulters sorted by balance using Bubble Sort
void getDefaulters() {
    DataRow dataset[MAX_ROWS];
    int count = readTXT("fees.txt", dataset);

    // Temp operational containers for processing structural rows
    DataRow defaulterRows[MAX_ROWS];
    double balances[MAX_ROWS];
    int defCount = 0;

    for (int i = 0; i < count; ++i) {
        double due = std::stod(dataset[i].cells[2]);
        double paid = std::stod(dataset[i].cells[3]);
        double balance = due - paid;

        if (balance > 0) {
            balances[defCount] = balance;
            defaulterRows[defCount] = dataset[i];
            defCount++;
        }
    }

    // Required Bubble Sort to structure output collections in descending order
    for (int i = 0; i < defCount - 1; ++i) {
        for (int j = 0; j < defCount - i - 1; ++j) {
            if (balances[j] < balances[j + 1]) {
                // Swap values
                double tempBal = balances[j];
                balances[j] = balances[j + 1];
                balances[j + 1] = tempBal;

                // Swap rows
                DataRow tempRow = defaulterRows[j];
                defaulterRows[j] = defaulterRows[j + 1];
                defaulterRows[j + 1] = tempRow;
            }
        }
    }

    std::cout << "\n--- Fee Defaulters List (Sorted by Outstanding Balance) ---\n";
    std::cout << "Roll Number  | Semester | Balance Outstanding\n";
    std::cout << "----------------------------------------------\n";
    for (int i = 0; i < defCount; ++i) {
        std::cout << defaulterRows[i].cells[0] << "  | " 
                  << defaulterRows[i].cells[1] << "     | Rs. " 
                  << balances[i] << "\n";
    }
}