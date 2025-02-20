#include "crow_server.h"

#include <crow.h>

#include "handlers/vehicle_handler.h"
#include "middleware/cors_middleware.h"

void runCrow() {
    crow::App<CORS_MIDDLEWARE> app;

    // 根路径（测试用）
    CROW_ROUTE(app, "/")([]() { return "EV Rental System Backend"; });

    // 车辆 API
    CROW_ROUTE(app, "/vehicles").methods(crow::HTTPMethod::GET)(getVehicleList);

    // 品牌 API
    CROW_ROUTE(app, "/brands").methods(crow::HTTPMethod::GET)(getBrandList);

    app.port(8081).multithreaded().run();
}