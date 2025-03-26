#ifndef USER_HANDLER_H
#define USER_HANDLER_H

#include <crow.h>

crow::response registerUser(const crow::request& req);
crow::response loginUser(const crow::request& req);
crow::response getUser(const crow::request& req);
crow::response updateUser(const crow::request& req);
crow::response getAllUser(const crow::request& req);

#endif  // USER_HANDLER_H
