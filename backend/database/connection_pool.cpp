#include "connection_pool.h"

#include <fstream>
#include <iostream>

#include "mysql.h"

// 连接池复用
struct ReuseDeleter {
    void operator()(MYSQL* p) const {
        if (ConnectionPool::getInstance().getIsReuse()) {
            ConnectionPool::getInstance().releaseConnection(p);
        } else {
            // 检查连接是否有效
            if (mysql_ping(p) != 0) {
                std::cerr << "连接无效，错误: " << mysql_error(p) << std::endl;
            } else {
                mysql_close(p);
            }
        }
    }
};

// 获取单例实例
ConnectionPool& ConnectionPool::getInstance() {
    static ConnectionPool instance;
    return instance;
}

// 构造函数
ConnectionPool::ConnectionPool() : maxConnections(10), currentConnections(0) {
    std::ifstream file(DBCONFIG_PATH);
    if (!file) {
        std::cerr << "无法打开配置文件: " << DBCONFIG_PATH << std::endl;
        exit(EXIT_FAILURE);
    }

    json config;
    file >> config;
    dbConfig = {config["host"].get<std::string>(), config["user"].get<std::string>(),
                config["password"].get<std::string>(), config["database"].get<std::string>()};

    isReuse = true;
    createConnections(maxConnections);
}

// 析构函数
ConnectionPool::~ConnectionPool() {
    isReuse = false;
    while (!pool.empty()) {
        pool.pop();
        currentConnections--;
    }
}

// 创建连接
void ConnectionPool::createConnections(int count) {
    std::lock_guard<std::mutex> lock(mutex_);
    for (int i = 0; i < count; ++i) {
        MYSQL* conn = mysql_init(nullptr);
        if (!conn) {
            std::cerr << "MySQL 初始化失败！" << std::endl;
            continue;
        }
        if (!mysql_real_connect(conn, dbConfig.host.c_str(), dbConfig.user.c_str(),
                                dbConfig.password.c_str(), dbConfig.database.c_str(), 0, nullptr,
                                0)) {
            std::cerr << "MySQL 连接失败: " << mysql_error(conn) << std::endl;
            mysql_close(conn);
            continue;
        }
        pool.push(std::shared_ptr<MYSQL>(conn, ReuseDeleter()));
        currentConnections++;
    }
}

// 获取连接
std::shared_ptr<MYSQL> ConnectionPool::getConnection() {
    std::unique_lock<std::mutex> lock(mutex_);
    condition_.wait(lock, [this]() { return !pool.empty(); });
    auto conn = pool.front();
    pool.pop();
    return conn;
}

// 释放连接
void ConnectionPool::releaseConnection(MYSQL* conn) {
    std::lock_guard<std::mutex> lock(mutex_);
    pool.push(std::shared_ptr<MYSQL>(conn, ReuseDeleter()));
    condition_.notify_one();
}