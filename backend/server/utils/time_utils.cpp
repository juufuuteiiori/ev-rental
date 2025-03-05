#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>

// 获取当前日期
std::string getCurrentDate() {
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    std::tm tm_now = *std::localtime(&time_t_now);

    // 格式化为 "YYYY-MM-DD"
    std::ostringstream oss;
    oss << std::put_time(&tm_now, "%Y-%m-%d");
    return oss.str();
}

// 将日期加上指定的月份
std::string addMonthsToDate(const std::string& date_str, int months) {
    std::tm tm_date = {};
    std::istringstream ss(date_str);
    ss >> std::get_time(&tm_date, "%Y-%m-%d");

    // 将月份加到日期上
    tm_date.tm_mon += months;
    mktime(&tm_date);  // 标准化日期

    std::ostringstream oss;
    oss << std::put_time(&tm_date, "%Y-%m-%d");
    return oss.str();
}