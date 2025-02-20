#include "database.h"

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>

using json = nlohmann::json;

struct DBConfig {
    std::string host;
    std::string user;
    std::string password;
    std::string database;
    std::string sql;
};

DBConfig loadConfig(const std::string& configPath) {
    std::ifstream file(configPath);
    if (!file) {
        std::cerr << "无法打开配置文件: " << configPath << std::endl;
        exit(EXIT_FAILURE);
    }

    json config;
    file >> config;

    return {config["host"].get<std::string>(), config["user"].get<std::string>(),
            config["password"].get<std::string>(), config["database"].get<std::string>(),
            config["sql"].get<std::string>()};
}

DBConfig dbConfig = loadConfig(DBCONFIG_PATH);

struct MysqlDeleter {
    void operator()(MYSQL* conn) const {
        if (conn) {
            mysql_close(conn);
            std::cout << "数据库连接已关闭" << std::endl;
        }
    }
};

// 使用 `std::shared_ptr` 作为全局数据库连接
std::shared_ptr<MYSQL> dbConnection = nullptr;

void initDatabaseConnection() {
    if (!dbConnection) {
        MYSQL* conn = mysql_init(nullptr);
        if (!conn) {
            std::cerr << "MySQL 初始化失败" << std::endl;
            exit(EXIT_FAILURE);
        }

        if (!mysql_real_connect(conn, dbConfig.host.c_str(), dbConfig.user.c_str(),
                                dbConfig.password.c_str(), dbConfig.database.c_str(), 0, nullptr,
                                0)) {
            std::cerr << "MySQL 连接失败: " << mysql_error(conn) << std::endl;
            mysql_close(conn);
            exit(EXIT_FAILURE);
        }
        dbConnection = std::shared_ptr<MYSQL>(conn, MysqlDeleter());  // 绑定智能指针
    }
}

std::shared_ptr<MYSQL> getDatabaseConnection() {
    if (!dbConnection) {
        initDatabaseConnection();
    }
    return dbConnection;
}

void initDatabase() {
    std::ifstream file(dbConfig.sql.c_str());
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