#ifndef DATABASE_H
#define DATABASE_H

#include "connection_pool.h"

void initDatabase();

inline std::shared_ptr<MYSQL> getDatabaseConnection() {
    return ConnectionPool::getInstance().getConnection();
}

#endif  // DATABASE_H
