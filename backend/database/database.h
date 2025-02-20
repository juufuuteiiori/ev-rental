#ifndef DATABASE_H
#define DATABASE_H

#include <mysql/mysql.h>

#include <memory>

void initDatabase();
std::shared_ptr<MYSQL> getDatabaseConnection();

#endif  // DATABASE_H
