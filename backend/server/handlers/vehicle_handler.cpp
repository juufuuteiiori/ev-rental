#include "vehicle_handler.h"

#include <database/database.h>
#include <server/utils/jwt_utils.h>
#include <server/utils/time_utils.h>

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

    vehicle["model_id"] = model_id;
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

    result["msg"] = "成功";
    result["data"] = std::move(vehicle);
    return crow::response(200, result);
}

crow::response submitModel(const crow::request& req) {
    crow::json::wvalue result;

    // 读取 Authorization 头部
    std::string authorization = req.get_header_value("Authorization");
    if (authorization.empty()) {
        result["msg"] = "缺少 Authorization 头部";
        return crow::response(400, result);
    }

    std::string token;
    if (authorization.find("Bearer ") == 0) {
        token = authorization.substr(7);  // 提取 token 部分
    } else {
        result["msg"] = "无效的 Authorization 格式";
        return crow::response(400, result);
    }

    // 验证 JWT
    auto jwt_result = validateJWT(token);
    if (!jwt_result) {
        result["msg"] = "无效的 token";
        return crow::response(401, result);  // 401 Unauthorized
    }

    auto jwt_role = jwt_result->second;
    if (jwt_role != "管理员") {
        result["msg"] = "非管理员无法上传品牌";
        return crow::response(401, result);
    }

    // 解析 JSON 请求
    auto body = crow::json::load(req.body);
    if (!body || !body.has("model_brand") || !body.has("model_name") || !body.has("imageUrls") ||
        !body.has("range") || !body.has("power_type") || !body.has("sale_price") ||
        !body.has("rental_price") || !body.has("charging_power") || !body.has("acceleration") ||
        !body.has("seats") || !body.has("storage_space")) {
        result["msg"] = "表单错误，缺少必填字段";
        return crow::response(400, result);
    }

    if (body["range"].t() != crow::json::type::Number ||
        body["sale_price"].t() != crow::json::type::Number ||
        body["rental_price"].t() != crow::json::type::Number ||
        body["charging_power"].t() != crow::json::type::Number ||
        body["acceleration"].t() != crow::json::type::Number ||
        body["seats"].t() != crow::json::type::Number ||
        body["storage_space"].t() != crow::json::type::Number) {
        result["msg"] = "表单错误：某些字段必须是数字";
        return crow::response(400, result);
    }

    std::string model_brand = body["model_brand"].s();
    std::string model_name = body["model_name"].s();
    std::string power_type = body["power_type"].s();

    std::vector<std::string> imageUrls;
    std::string image_paths;
    if (body.has("imageUrls")) {
        auto imageArray = body["imageUrls"];
        for (const auto& item : imageArray) {
            if (item.t() == crow::json::type::String) {
                if (!image_paths.empty()) image_paths += ",";
                image_paths += item.s();
            }
        }
        std::cout << image_paths << std::endl;
    }

    int range = body["range"].i();
    int sale_price = body["sale_price"].i();
    int rental_price = body["rental_price"].i();
    int charging_power = body["charging_power"].i();
    int acceleration = body["acceleration"].i();
    int seats = body["seats"].i();
    int storage_space = body["storage_space"].i();

    // 连接数据库
    auto conn = getDatabaseConnection();
    if (!conn) {
        result["code"] = 0;
        result["msg"] = "数据库连接失败";
        return crow::response(500, result);
    }

    char safe_brand[100], safe_name[100], safe_type[100], safe_image[256];
    mysql_real_escape_string(conn.get(), safe_brand, model_brand.c_str(), model_brand.length());
    mysql_real_escape_string(conn.get(), safe_name, model_name.c_str(), model_name.length());
    mysql_real_escape_string(conn.get(), safe_type, power_type.c_str(), power_type.length());
    mysql_real_escape_string(conn.get(), safe_image, image_paths.c_str(), image_paths.length());

    std::string insert_query =
        "INSERT INTO models (brand_name, model_name, power_type, max_range, leasing_price, "
        "purchase_price, peak_power, acceleration, seat_count, storage_space, image_paths) "
        "VALUES ('" +
        std::string(safe_brand) + "', '" + std::string(safe_name) + "', '" +
        std::string(safe_type) + "', " + std::to_string(range) + ", " +
        std::to_string(rental_price) + ", " + std::to_string(sale_price) + ", " +
        std::to_string(charging_power) + ", " + std::to_string(acceleration) + ", " +
        std::to_string(seats) + ", " + std::to_string(storage_space) + ", '" +
        std::string(safe_image) + "');";

    // std::cout << insert_query << std::endl;

    if (mysql_query(conn.get(), insert_query.c_str()) != 0) {
        result["msg"] = "数据库错误";
        return crow::response(500, result);
    }

    int model_id = mysql_insert_id(conn.get());
    std::string date = getCurrentDate();
    std::string insert_query_2 =
        "INSERT INTO model_price_history (model_id, record_date, sale_price, rental_price) VALUES "
        "(" +
        std::to_string(model_id) + ", '" + date + "', " + std::to_string(sale_price) + ", " +
        std::to_string(rental_price) + ");";

    // std::cout << insert_query_2 << std::endl;

    if (mysql_query(conn.get(), insert_query_2.c_str()) != 0) {
        result["msg"] = "数据库错误";
        return crow::response(500, result);
    }

    result["msg"] = "上传成功";
    return crow::response(200, result);
}

crow::response updateModel(const crow::request& req) {
    crow::json::wvalue result;

    // 读取 Authorization 头部
    std::string authorization = req.get_header_value("Authorization");
    if (authorization.empty()) {
        result["msg"] = "缺少 Authorization 头部";
        return crow::response(400, result);
    }

    std::string token;
    if (authorization.find("Bearer ") == 0) {
        token = authorization.substr(7);  // 提取 token 部分
    } else {
        result["msg"] = "无效的 Authorization 格式";
        return crow::response(400, result);
    }

    // 验证 JWT
    auto jwt_result = validateJWT(token);
    if (!jwt_result) {
        result["msg"] = "无效的 token";
        return crow::response(401, result);  // 401 Unauthorized
    }

    auto jwt_role = jwt_result->second;
    if (jwt_role != "管理员") {
        result["msg"] = "非管理员无法上传品牌";
        return crow::response(401, result);
    }

    // 解析 JSON 请求
    auto body = crow::json::load(req.body);
    if (!body || !body.has("model_id") || !body.has("model_brand") || !body.has("model_name") ||
        !body.has("imageUrls") || !body.has("range") || !body.has("power_type") ||
        !body.has("sale_price") || !body.has("rental_price") || !body.has("charging_power") ||
        !body.has("acceleration") || !body.has("seats") || !body.has("storage_space")) {
        result["msg"] = "表单错误，缺少必填字段";
        return crow::response(400, result);
    }

    if (body["model_id"].t() != crow::json::type::Number ||
        body["range"].t() != crow::json::type::Number ||
        body["sale_price"].t() != crow::json::type::Number ||
        body["rental_price"].t() != crow::json::type::Number ||
        body["charging_power"].t() != crow::json::type::Number ||
        body["acceleration"].t() != crow::json::type::Number ||
        body["seats"].t() != crow::json::type::Number ||
        body["storage_space"].t() != crow::json::type::Number) {
        result["msg"] = "表单错误：某些字段必须是数字";
        return crow::response(400, result);
    }

    std::string model_brand = body["model_brand"].s();
    std::string model_name = body["model_name"].s();
    std::string power_type = body["power_type"].s();

    std::vector<std::string> imageUrls;
    std::string image_paths;
    if (body.has("imageUrls")) {
        auto imageArray = body["imageUrls"];
        for (const auto& item : imageArray) {
            if (item.t() == crow::json::type::String) {
                if (!image_paths.empty()) image_paths += ",";
                image_paths += item.s();
            }
        }
        std::cout << image_paths << std::endl;
    }

    int model_id = body["model_id"].i();
    int range = body["range"].i();
    int sale_price = body["sale_price"].i();
    int rental_price = body["rental_price"].i();
    int charging_power = body["charging_power"].i();
    int acceleration = body["acceleration"].i();
    int seats = body["seats"].i();
    int storage_space = body["storage_space"].i();

    // 连接数据库
    auto conn = getDatabaseConnection();
    if (!conn) {
        result["code"] = 0;
        result["msg"] = "数据库连接失败";
        return crow::response(500, result);
    }

    char safe_brand[100], safe_name[100], safe_type[100], safe_image[256];
    mysql_real_escape_string(conn.get(), safe_brand, model_brand.c_str(), model_brand.length());
    mysql_real_escape_string(conn.get(), safe_name, model_name.c_str(), model_name.length());
    mysql_real_escape_string(conn.get(), safe_type, power_type.c_str(), power_type.length());
    mysql_real_escape_string(conn.get(), safe_image, image_paths.c_str(), image_paths.length());

    std::string update_query = "UPDATE models SET brand_name = '" + std::string(safe_brand) +
                               "', model_name = '" + std::string(safe_name) + "', power_type = '" +
                               std::string(safe_type) + "', max_range = " + std::to_string(range) +
                               ", leasing_price = " + std::to_string(rental_price) +
                               ", purchase_price = " + std::to_string(sale_price) +
                               ", peak_power = " + std::to_string(charging_power) +
                               ", acceleration = " + std::to_string(acceleration) +
                               ", seat_count = " + std::to_string(seats) +
                               ", storage_space = " + std::to_string(storage_space) +
                               ", image_paths = '" + std::string(safe_image) +
                               "' WHERE model_id = " + std::to_string(model_id) + ";";

    std::cout << update_query << std::endl;

    if (mysql_query(conn.get(), update_query.c_str()) != 0) {
        result["msg"] = "数据库错误";
        return crow::response(500, result);
    }

    std::string date = getCurrentDate();
    std::string upsert_query =
        "INSERT INTO model_price_history (model_id, record_date, sale_price, rental_price) VALUES "
        "(" +
        std::to_string(model_id) + ", '" + date + "', " + std::to_string(sale_price) + ", " +
        std::to_string(rental_price) +
        ") "
        "ON DUPLICATE KEY UPDATE "
        "sale_price = VALUES(sale_price), rental_price = VALUES(rental_price);";

    std::cout << upsert_query << std::endl;

    if (mysql_query(conn.get(), upsert_query.c_str()) != 0) {
        result["msg"] = "数据库错误";
        return crow::response(500, result);
    }

    result["msg"] = "修改成功";
    return crow::response(200, result);
}