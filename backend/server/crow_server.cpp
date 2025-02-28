#include "crow_server.h"

#include <crow.h>

#include <csignal>

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

    // 获取推荐车辆信息 API
    CROW_ROUTE(app, "/recommended").methods(crow::HTTPMethod::GET)(getRecommendedVehicleList);

    CROW_ROUTE(app, "/static/<string>")
        .methods(crow::HTTPMethod::GET)([](const crow::request&, std::string filename) {
            // std::string path = "backend/assets/images/" + filename;
            std::string path = std::string(ASSETS_PATH) + "/" + filename;
            std::ifstream file(path, std::ios::binary);

            if (!file) {
                return crow::response(404, "File not found");
            }

            std::ostringstream buffer;
            buffer << file.rdbuf();
            crow::response res(buffer.str());
            res.set_header("Content-Type", "image/png");
            return res;
        });

    app.port(8081).multithreaded().run();

    g_app = nullptr;
}