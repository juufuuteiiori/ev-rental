#ifndef ORDER_HANDLER_H
#define ORDER_HANDLER_H

#include <crow.h>
#include <server/utils/time_utils.h>

crow::response getOrders(const crow::request& req);
crow::response getOrderById(const crow::request& req);
crow::response submitOrder(const crow::request& req);
crow::response orderDone(const crow::request& req);
crow::response submitComment(const crow::request& req);

#endif  // ORDER_HANDLER_H
