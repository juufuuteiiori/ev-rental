#include "vehicle_handler.h"

#include <database/database.h>

// 查询品牌列表信息
crow::response getBrandList() {
    crow::json::wvalue result;

    auto conn = getDatabaseConnection();
    if (!conn) {
        result["code"] = 0;
        result["msg"] = "数据库连接失败";
        return crow::response(500, result);
    }

    std::string query = "SELECT DISTINCT brand_name FROM models";
    if (mysql_query(conn.get(), query.c_str())) {
        result["code"] = 0;
        result["msg"] = "SQL 执行失败: " + std::string(mysql_error(conn.get()));
        return crow::response(500, result);
    }

    // 获取查询结果
    std::shared_ptr<MYSQL_RES> res(mysql_store_result(conn.get()), mysql_free_result);
    if (!res) {
        result["code"] = 0;
        result["msg"] = "查询结果为空";
        return crow::response(500, result);
    }

    // 遍历结果集
    std::vector<crow::json::wvalue> brands;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res.get()))) {
        brands.push_back(row[0]);
    }

    result["code"] = 1;
    result["msg"] = "成功";
    result["data"] = std::move(brands);

    return crow::response(200, result);
}

// 查询车辆列表信息
crow::response getVehicleList() {
    crow::json::wvalue result;

    auto conn = getDatabaseConnection();
    if (!conn) {
        result["code"] = 0;
        result["msg"] = "数据库连接失败";
        return crow::response(500, result);
    }

    std::string query =
        "SELECT model_id, brand_name, model_name, max_range, leasing_price, purchase_price FROM "
        "models";
    if (mysql_query(conn.get(), query.c_str())) {
        result["code"] = 0;
        result["msg"] = "SQL 执行失败: " + std::string(mysql_error(conn.get()));
        return crow::response(500, result);
    }

    // 获取查询结果
    std::shared_ptr<MYSQL_RES> res(mysql_store_result(conn.get()), mysql_free_result);
    if (!res) {
        result["code"] = 0;
        result["msg"] = "查询结果为空";
        return crow::response(500, result);
    }

    // 遍历结果集
    std::vector<crow::json::wvalue> vehicles;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res.get()))) {
        crow::json::wvalue vehicle;
        vehicle["model_id"] = row[0] ? std::stoi(row[0]) : 0;
        vehicle["brand"] = row[1] ? row[1] : "";
        vehicle["model"] = row[2] ? row[2] : "";
        vehicle["range"] = row[3] ? std::stoi(row[3]) : 0;
        vehicle["rentalPrice"] = row[4] ? std::stod(row[4]) : 0.0;
        vehicle["salePrice"] = row[5] ? std::stod(row[5]) : 0.0;

        vehicles.push_back(std::move(vehicle));
    }

    result["code"] = 1;
    result["msg"] = "成功";
    result["data"] = std::move(vehicles);

    return crow::response(200, result);
}
