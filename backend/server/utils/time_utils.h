#ifndef TIME_UTILS_H
#define TIME_UTILS_H

#include <string>

std::string getCurrentDate();
std::string addMonthsToDate(const std::string& date_str, int months);

#endif  // TIME_UTILS_H
