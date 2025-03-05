#include "crow_server.h"

#include <crow.h>

#include <csignal>

#include "handlers/image_handler.h"
#include "handlers/order_handler.h"
#include "handlers/user_handler.h"
#include "handlers/vehicle_handler.h"
#include "middleware/cors_middleware.h"

// 全局指针，保存 Crow App 的地址
crow::App<CORS_MIDDLEWARE>* g_app = nullptr;

// 信号处理函数
void signalHandler(int signum) {
    if (g_app) {
        g_app->stop();
    }
}

void runCrow() {
    crow::App<CORS_MIDDLEWARE> app;

    g_app = &app;
    // 注册 SIGINT (Ctrl+C) 和 SIGTERM 信号处理器
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    // 根路径（测试用）
    CROW_ROUTE(app, "/")([]() { return "EV Rental System Backend"; });

    // 车辆 API
    CROW_ROUTE(app, "/vehicles").methods(crow::HTTPMethod::GET)(getVehicleList);

    // 车辆详情 API
    CROW_ROUTE(app, "/model").methods(crow::HTTPMethod::GET)(getVehicleDetails);

    // 品牌 API
    CROW_ROUTE(app, "/brands").methods(crow::HTTPMethod::GET)(getBrandList);

    // 账号注册 API
    CROW_ROUTE(app, "/register").methods(crow::HTTPMethod::POST)(registerUser);

    // 账号登录 API
    CROW_ROUTE(app, "/login").methods(crow::HTTPMethod::POST)(loginUser);

    // 获取账号信息 API
    CROW_ROUTE(app, "/user").methods(crow::HTTPMethod::GET)(getUser);

    // 修改账号信息 API
    CROW_ROUTE(app, "/user").methods(crow::HTTPMethod::POST)(updateUser);

    // 获取账号订单信息 API
    CROW_ROUTE(app, "/orders").methods(crow::HTTPMethod::GET)(getOrders);

    // 获取订单信息 API
    CROW_ROUTE(app, "/order").methods(crow::HTTPMethod::GET)(getOrderById);

    // 提交订单信息 API
    CROW_ROUTE(app, "/orders").methods(crow::HTTPMethod::POST)(submitOrder);

    // 获取推荐车辆信息 API
    CROW_ROUTE(app, "/recommended").methods(crow::HTTPMethod::GET)(getRecommendedVehicleList);

    // 获取图片 API
    CROW_ROUTE(app, "/image").methods(crow::HTTPMethod::GET)(getImage);

    app.port(8081).multithreaded().run();

    g_app = nullptr;
}