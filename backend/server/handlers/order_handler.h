#ifndef ORDER_HANDLER_H
#define ORDER_HANDLER_H

#include <crow.h>

crow::response getOrders(const crow::request& req);
crow::response getOrderById(const crow::request& req);

#endif  // ORDER_HANDLER_H
