#ifndef CONNECTION_POOL_H
#define CONNECTION_POOL_H

#include <mysql/mysql.h>

#include <condition_variable>
#include <memory>
#include <nlohmann/json.hpp>
#include <queue>

using json = nlohmann::json;

struct DBConfig {
    std::string host;
    std::string user;
    std::string password;
    std::string database;
};

class ConnectionPool {
   public:
    static ConnectionPool& getInstance();    // 单例模式获取连接池
    std::shared_ptr<MYSQL> getConnection();  // 获取连接
    void releaseConnection(MYSQL* conn);     // 释放连接
    bool getIsReuse() { return isReuse; };   // 是否复用

   private:
    ConnectionPool();
    ~ConnectionPool();

    void createConnections(int count);        // 创建连接
    int maxConnections;                       // 最大连接数
    int currentConnections;                   // 当前已分配的连接数
    bool isReuse;                             // 是否复用
    DBConfig dbConfig;                        // 数据库配置信息
    std::queue<std::shared_ptr<MYSQL>> pool;  // 连接池
    std::mutex mutex_;
    std::condition_variable condition_;
};

#endif  // CONNECTION_POOL_H
