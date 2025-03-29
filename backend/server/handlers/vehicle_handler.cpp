#include "vehicle_handler.h"

#include <database/database.h>
#include <server/utils/utils.h>

#include <regex>

crow::response getVehicleList(const crow::request& req) {
    crow::json::wvalue result;
    crow::response response;

    auto jwt_result = getJWT(req, response);
    if (!jwt_result) {
        return response;
    }

    auto jwt_role = jwt_result->second;
    if (jwt_role != "管理员") {
        result["msg"] = "非管理员无法获取车辆列表信息";
        return crow::response(401, result);
    }

    // 连接数据库
    auto conn = getDatabaseConnection();
    if (!conn) {
        result["msg"] = "数据库连接失败";
        return crow::response(500, result);
    }

    std::string query =
        "SELECT v.vehicle_id, m.brand_name, m.model_name, v.license_plate, v.purchase_date, "
        "v.status FROM vehicles v LEFT JOIN models m ON m.model_id = v.model_id";

    if (mysql_query(conn.get(), query.c_str()) != 0) {
        result["msg"] = "SQL 执行失败: " + std::string(mysql_error(conn.get()));
        return crow::response(500, result);
    }

    // 获取查询结果
    std::shared_ptr<MYSQL_RES> res(mysql_store_result(conn.get()), mysql_free_result);
    if (!res || mysql_num_rows(res.get()) == 0) {
        result["msg"] = "查询结果为空";
        return crow::response(200, result);
    }

    // 遍历结果集
    std::vector<crow::json::wvalue> vehicles;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res.get()))) {
        crow::json::wvalue vehicle;
        vehicle["vehicle_id"] = std::stoi(row[0]);
        vehicle["brand"] = row[1];
        vehicle["model"] = row[2];
        vehicle["license_plate"] = row[3];
        vehicle["date"] = row[4];
        vehicle["status"] = row[5];
        vehicles.push_back(std::move(vehicle));
    }
    result["vehicles"] = std::move(vehicles);

    result["msg"] = "成功";
    return crow::response(200, result);
}

crow::response submitVehicle(const crow::request& req) {
    crow::json::wvalue result;
    crow::response response;

    auto jwt_result = getJWT(req, response);
    if (!jwt_result) {
        return response;
    }

    auto jwt_role = jwt_result->second;
    if (jwt_role != "管理员") {
        result["msg"] = "非管理员无法上传品牌";
        return crow::response(401, result);
    }

    auto body = crow::json::load(req.body);
    if (!body || !body.has("model_id") || body["model_id"].t() != crow::json::type::Number ||
        !body.has("status") || body["status"].t() != crow::json::type::String ||
        !body.has("license_plate") || body["license_plate"].t() != crow::json::type::String) {
        result["msg"] = "表单错误";
        return crow::response(400, result);
    }

    int model_id = body["model_id"].i();
    std::string status = body["status"].s();
    std::string license_plate = body["license_plate"].s();
    std::regex new_energy_plate(R"(^[A-HJ-NP-Z][A-HJ-NP-Z0-9]{6}$)");

    if (license_plate.substr(0, 3) != "川" ||
        std::regex_match(license_plate.substr(3, license_plate.size() - 3), new_energy_plate) ==
            false) {
        result["msg"] = "车牌号不符合逻辑";
        return crow::response(400, result);
    }

    // 连接数据库
    auto conn = getDatabaseConnection();
    if (!conn) {
        result["msg"] = "数据库连接失败";
        return crow::response(500, result);
    }

    char safe_status[255], safe_license_plate[255];
    mysql_real_escape_string(conn.get(), safe_status, status.c_str(), status.length());
    mysql_real_escape_string(conn.get(), safe_license_plate, license_plate.c_str(),
                             license_plate.length());

    std::string insert_query = "INSERT INTO vehicles (model_id, license_plate, status) VALUES (" +
                               std::to_string(model_id) + ", '" + std::string(safe_license_plate) +
                               "', '" + std::string(safe_status) + "');";

    if (mysql_query(conn.get(), insert_query.c_str()) != 0) {
        result["msg"] = "数据库错误";
        return crow::response(500, result);
    }

    result["msg"] = "上传成功";
    return crow::response(200, result);
}

crow::response updateVehicle(const crow::request& req) {
    crow::json::wvalue result;
    crow::response response;

    auto jwt_result = getJWT(req, response);
    if (!jwt_result) {
        return response;
    }

    auto jwt_role = jwt_result->second;
    if (jwt_role != "管理员") {
        result["msg"] = "非管理员无法修改车辆状态";
        return crow::response(401, result);
    }

    auto body = crow::json::load(req.body);
    if (!body || !body.has("vehicle_id") || body["vehicle_id"].t() != crow::json::type::Number ||
        !body.has("status") || body["status"].t() != crow::json::type::String) {
        result["msg"] = "表单信息错误";
        return crow::response(400, result);
    }

    auto vehicle_id = body["vehicle_id"].i();
    std::string status = body["status"].s();

    if (status != "可用" && status != "不可用") {
        result["msg"] = "表单错误";
        return crow::response(400, result);
    }

    // 连接数据库
    auto conn = getDatabaseConnection();
    if (!conn) {
        result["msg"] = "数据库连接失败";
        return crow::response(500, result);
    }

    std::string query = "SELECT * FROM vehicles WHERE vehicle_id = " + std::to_string(vehicle_id) +
                        " AND status = '" + status + "'";

    if (mysql_query(conn.get(), query.c_str()) != 0) {
        result["msg"] = "查询语句错误";
        return crow::response(500, result);
    }

    std::shared_ptr<MYSQL_RES> queryRes(mysql_store_result(conn.get()), mysql_free_result);
    if (!queryRes || mysql_num_rows(queryRes.get()) == 0) {
        result["msg"] = "数据不存在";
        return crow::response(409, result);
    }

    status = status == "可用" ? "不可用" : "可用";

    std::string update_query = "UPDATE vehicles SET status = '" + status +
                               "' WHERE vehicle_id = " + std::to_string(vehicle_id);

    if (mysql_query(conn.get(), update_query.c_str()) != 0) {
        result["msg"] = "修改语句错误";
        return crow::response(500, result);
    }

    result["msg"] = "修改成功";
    return crow::response(200, result);
}

crow::response delVehicle(const crow::request& req) {
    crow::json::wvalue result;
    crow::response response;

    auto jwt_result = getJWT(req, response);
    if (!jwt_result) {
        return response;
    }

    auto jwt_role = jwt_result->second;
    if (jwt_role != "管理员") {
        result["msg"] = "非管理员无法删除车辆";
        return crow::response(401, result);
    }

    result["msg"] = "未实现删除车辆功能";
    return crow::response(200, result);
}