#include "database.h"

#include <fstream>
#include <iostream>

void initDatabase() {
    std::ifstream file("../database/database.sql");
    if (!file) {
        std::cerr << "无法打开 SQL 文件" << std::endl;
        return;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();  // 读取整个 SQL 文件内容

    auto conn = getDatabaseConnection();  // 复用数据库连接
    if (!conn) {
        std::cerr << "数据库连接获取失败" << std::endl;
        return;
    }

    std::string query;
    while (std::getline(buffer, query, ';')) {  // 按分号拆分 SQL 语句
        // 去掉前后空格
        query.erase(0, query.find_first_not_of(" \t\n\r"));
        query.erase(query.find_last_not_of(" \t\n\r") + 1);

        if (!query.empty()) {  // 避免执行空语句
            if (mysql_query(conn.get(), query.c_str())) {
                std::cerr << "SQL 执行失败: " << mysql_error(conn.get()) << " " << query
                          << std::endl;
            }
        }
    }
}