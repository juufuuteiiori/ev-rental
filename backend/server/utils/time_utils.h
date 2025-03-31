#ifndef TIME_UTILS_H
#define TIME_UTILS_H

#include <string>

std::string getCurrentDate();
std::string addMonthsToDate(const std::string& date_str, int months);
std::string addDaysToDate(const std::string& date_str, int days);
int getDiffDay(const std::string& date_str);

#endif  // TIME_UTILS_H
