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

// 计算天数差 date_str "YYYY-MM-DD" （字符串中有引号）
int getDiffDay(const std::string& date_str) {
    // 获取当前时间
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);

    // 将当前时间转换为 tm 结构
    std::tm* now_tm = std::localtime(&now_time);
    now_tm->tm_hour = 0;
    now_tm->tm_min = 0;
    now_tm->tm_sec = 0;

    // 获取今天的时间点（按天计算）
    auto today = std::chrono::time_point_cast<std::chrono::system_clock::duration>(now);

    // 解析传入的日期字符串 (YYYY-MM-DD)
    std::tm comment_tm = {};
    std::istringstream ss(date_str.substr(1, date_str.length() - 2));
    ss >> std::get_time(&comment_tm, "%Y-%m-%d");

    // 将解析后的时间转换为 time_t 类型
    std::time_t comment_time = std::mktime(&comment_tm);

    // 将 time_t 转换为 std::chrono 时间类型
    auto comment_day = std::chrono::system_clock::from_time_t(comment_time);

    // 获取时间间隔
    auto duration = now - comment_day;

    // 计算时间差
    auto days_diff =
        std::chrono::duration_cast<std::chrono::seconds>(duration).count() / 86400;  // 转换为天数差

    return days_diff;  // 返回天数差
}