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

crow::response getRecommendedVehicleList() {
    crow::json::wvalue result;

    auto conn = getDatabaseConnection();
    if (!conn) {
        result["code"] = 0;
        result["msg"] = "数据库连接失败";
        return crow::response(500, result);
    }

    std::string query =
        "SELECT model_id, brand_name, model_name, max_range, leasing_price, purchase_price, "
        "image_paths FROM "
        "models WHERE recommend = TRUE";

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
    std::vector<crow::json::wvalue> recommendedVehicles;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res.get()))) {
        std::string firstImage = "";

        if (row[6] && std::strlen(row[6]) > 0) {
            std::istringstream ss(row[6]);
            std::getline(ss, firstImage, ',');
        }

        crow::json::wvalue vehicle;
        vehicle["model_id"] = row[0] ? std::stoi(row[0]) : 0;
        vehicle["brand_name"] = row[1] ? row[1] : "";
        vehicle["model_name"] = row[2] ? row[2] : "";
        vehicle["range"] = row[3] ? std::stoi(row[3]) : 0;
        vehicle["rentalPrice"] = row[4] ? std::stod(row[4]) : 0.0;
        vehicle["salePrice"] = row[5] ? std::stod(row[5]) : 0.0;
        vehicle["image"] = firstImage;

        recommendedVehicles.push_back(std::move(vehicle));
    }

    result["code"] = 1;
    result["msg"] = "成功";
    result["data"] = std::move(recommendedVehicles);

    return crow::response(200, result);
}

crow::response getVehicleDetails(const crow::request& req) {
    crow::json::wvalue result;

    auto params = req.url_params;
    std::string model_id = params.get("model_id");

    if (model_id.empty()) {
        result["code"] = 0;
        result["msg"] = "缺少型号编号";
        return crow::response(400, result);
    }

    // 连接数据库
    auto conn = getDatabaseConnection();
    if (!conn) {
        result["code"] = 0;
        result["msg"] = "数据库连接失败";
        return crow::response(500, result);
    }

    // 防止 SQL 注入
    char safe_id[100];
    mysql_real_escape_string(conn.get(), safe_id, model_id.c_str(), model_id.length());

    crow::json::wvalue vehicle;

    // 查询基本信息
    std::string query =
        "SELECT brand_name, model_name, max_range, leasing_price, purchase_price, "
        "power_type, peak_power, acceleration, seat_count, storage_space, image_paths FROM models "
        "WHERE model_id = '" +
        std::string(safe_id) + "'";

    if (mysql_query(conn.get(), query.c_str())) {
        result["code"] = 0;
        result["msg"] = "SQL 执行失败: " + std::string(mysql_error(conn.get()));
        return crow::response(500, result);
    }

    std::shared_ptr<MYSQL_RES> res(mysql_store_result(conn.get()), mysql_free_result);
    if (!res) {
        result["code"] = 0;
        result["msg"] = "查询结果为空";
        return crow::response(500, result);
    }

    auto row = mysql_fetch_row(res.get());

    vehicle["brand_name"] = row[0];
    vehicle["model_name"] = row[1];
    vehicle["max_range"] = std::stoi(row[2]);
    vehicle["leasing_price"] = std::stod(row[3]);
    vehicle["purchase_price"] = std::stod(row[4]);
    vehicle["power_type"] = row[5];
    vehicle["peak_power"] = std::stod(row[6]);
    vehicle["acceleration"] = std::stod(row[7]);
    vehicle["seat_count"] = std::stoi(row[8]);
    vehicle["storage_space"] = std::stoi(row[9]);

    std::vector<crow::json::wvalue> imageListJson;
    std::istringstream ss(row[10]);
    std::string image;
    while (std::getline(ss, image, ',')) {
        image.erase(image.begin(), std::find_if(image.begin(), image.end(), [](unsigned char ch) {
                        return !std::isspace(ch);
                    }));
        image.erase(std::find_if(image.rbegin(), image.rend(),
                                 [](unsigned char ch) { return !std::isspace(ch); })
                        .base(),
                    image.end());
        if (!image.empty()) imageListJson.push_back(image);
    }
    vehicle["image_paths"] = std::move(imageListJson);

    // 查询可用数量
    std::string count_query = "SELECT COUNT(*) FROM vehicles WHERE model_id = '" +
                              std::string(safe_id) + "' AND status = '可用'";
    if (mysql_query(conn.get(), count_query.c_str())) {
        result["code"] = 0;
        result["msg"] = "SQL 执行失败: " + std::string(mysql_error(conn.get()));
        return crow::response(500, result);
    }

    std::shared_ptr<MYSQL_RES> res2(mysql_store_result(conn.get()), mysql_free_result);
    if (!res2) {
        result["code"] = 0;
        result["msg"] = "查询结果为空";
        return crow::response(500, result);
    }
    auto row2 = mysql_fetch_row(res2.get());
    vehicle["available_number"] = std::stoi(row2[0]);

    // 查询历史价格
    std::string price_query =
        "SELECT record_date, sale_price, rental_price FROM model_price_history WHERE model_id = '" +
        std::string(safe_id) + "' ORDER BY record_date ASC";

    if (mysql_query(conn.get(), price_query.c_str())) {
        result["code"] = 0;
        result["msg"] = "SQL 执行失败: " + std::string(mysql_error(conn.get()));
        return crow::response(500, result);
    }

    std::shared_ptr<MYSQL_RES> res3(mysql_store_result(conn.get()), mysql_free_result);
    if (!res3) {
        result["code"] = 0;
        result["msg"] = "查询结果为空";
        return crow::response(500, result);
    }

    std::vector<crow::json::wvalue> history_prices;
    MYSQL_ROW row3;
    while ((row3 = mysql_fetch_row(res3.get()))) {
        crow::json::wvalue price_entry;
        price_entry["date"] = row3[0];                    // 记录日期
        price_entry["salePrice"] = std::stod(row3[1]);    // 购车价格
        price_entry["rentalPrice"] = std::stod(row3[2]);  // 租赁价格
        history_prices.push_back(std::move(price_entry));
    }
    vehicle["history_prices"] = std::move(history_prices);

    result["code"] = 1;
    result["msg"] = "成功";
    result["data"] = std::move(vehicle);
    return crow::response(200, result);
}