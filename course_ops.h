#ifndef COURSE_OPS_H
#define COURSE_OPS_H

#include <string>
#include "filehandler.h"

// Enum to clearly flag success or reasons for enrollment failure
enum EnrollResult {
    ENROLL_SUCCESS,
    STUDENT_INACTIVE,
    COURSE_NOT_FOUND,
    NO_SEATS,
    ALREADY_ENROLLED,
    CREDIT_LIMIT_EXCEEDED,
    PREREQ_FAILED
};

// Required function signatures from specification
EnrollResult enrollStudent(const std::string& roll, const std::string& courseCode, const std::string& semester);
bool dropCourse(const std::string& roll, const std::string& courseCode, const std::string& semester);
int getCreditLoad(const std::string& roll, const std::string& semester);
bool checkPrerequisite(const std::string& roll, const std::string& courseCode);
void listenEnrolledStudents(const std::string& courseCode);

#endif