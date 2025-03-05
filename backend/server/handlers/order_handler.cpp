#include "order_handler.h"

#include <database/database.h>
#include <server/utils/jwt_utils.h>

crow::response getOrders(const crow::request& req) {
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
    auto jwt_user_id = validateJWT(token);
    if (!jwt_user_id) {
        result["msg"] = "无效的 token";
        return crow::response(401, result);  // 401 Unauthorized
    }

    // 获取 `user_id` 参数
    auto user_id = req.url_params.get("user_id");
    if (!user_id) {
        result["msg"] = "缺少用户ID";
        return crow::response(400, result);
    }

    if (jwt_user_id != user_id) {
        result["msg"] = "没有权限访问其他用户订单";
        return crow::response(400, result);
    }

    // 连接数据库
    auto conn = getDatabaseConnection();
    if (!conn) {
        result["msg"] = "数据库错误";
        return crow::response(500, result);
    }

    // 防止 SQL 注入
    char safe_id[10];
    mysql_real_escape_string(conn.get(), safe_id, user_id, strlen(user_id));

    // 查询订单信息
    std::string query =
        "SELECT o.order_id, o.order_date, m.brand_name, m.model_name, v.license_plate, "
        "o.order_type, o.status "
        "FROM orders o "
        "JOIN vehicles v ON o.vehicle_id = v.vehicle_id "
        "JOIN models m ON v.model_id = m.model_id "
        "WHERE o.user_id = '" +
        std::string(safe_id) + "'";

    if (mysql_query(conn.get(), query.c_str()) != 0) {
        result["msg"] = "数据库错误";
        return crow::response(500, result);
    }

    std::shared_ptr<MYSQL_RES> res(mysql_store_result(conn.get()), mysql_free_result);
    if (!res) {
        result["msg"] = "查询结果为空";
        return crow::response(200, result);
    }

    // 遍历结果集
    std::vector<crow::json::wvalue> orders;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res.get()))) {
        crow::json::wvalue order;
        order["order_id"] = row[0];
        order["order_date"] = row[1];
        order["brand_name"] = row[2];
        order["model_name"] = row[3];
        order["license_plate"] = row[4];
        order["order_type"] = row[5];
        order["order_status"] = row[6];
        orders.push_back(std::move(order));
    }
    result["orders"] = std::move(orders);

    return crow::response(200, result);
}

crow::response getOrderById(const crow::request& req) {
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
    auto jwt_user_id = validateJWT(token);
    if (!jwt_user_id) {
        result["msg"] = "无效的 token";
        return crow::response(401, result);  // 401 Unauthorized
    }

    // 获取 `order_id` 参数
    auto order_id = req.url_params.get("order_id");
    if (!order_id) {
        result["msg"] = "缺少订单ID";
        return crow::response(400, result);
    }

    // 连接数据库
    auto conn = getDatabaseConnection();
    if (!conn) {
        result["msg"] = "数据库错误";
        return crow::response(500, result);
    }

    // 防止 SQL 注入
    char safe_id[10];
    mysql_real_escape_string(conn.get(), safe_id, order_id, strlen(order_id));

    // 查询订单信息
    std::string query =
        "SELECT o.user_id, o.order_date, m.brand_name, m.model_name, v.license_plate, "
        "o.order_type, o.end_date, o.total_price, o.status "
        "FROM orders o "
        "JOIN vehicles v ON o.vehicle_id = v.vehicle_id "
        "JOIN models m ON v.model_id = m.model_id "
        "WHERE o.order_id = '" +
        std::string(safe_id) + "'";

    if (mysql_query(conn.get(), query.c_str()) != 0) {
        result["msg"] = "数据库查询错误";
        return crow::response(500, result);
    }

    std::shared_ptr<MYSQL_RES> res(mysql_store_result(conn.get()), mysql_free_result);
    if (!res || mysql_num_rows(res.get()) == 0) {
        result["msg"] = "查询结果为空";
        return crow::response(200, result);
    }

    crow::json::wvalue order;
    MYSQL_ROW row = mysql_fetch_row(res.get());

    auto user_id = row[0];
    if (jwt_user_id.value() != std::string(user_id)) {
        result["msg"] = "没有权限访问其他用户订单";
        return crow::response(400, result);
    }

    order["order_date"] = row[1];
    order["brand_name"] = row[2];
    order["model_name"] = row[3];
    order["license_plate"] = row[4];
    order["order_type"] = row[5];
    order["end_date"] = row[6];
    order["total_price"] = row[7];
    order["order_status"] = row[8];

    result["order"] = std::move(order);

    return crow::response(200, result);
}

crow::response submitOrder(const crow::request& req) {
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
    auto jwt_user_id = validateJWT(token);
    if (!jwt_user_id) {
        result["msg"] = "无效的 token";
        return crow::response(401, result);  // 401 Unauthorized
    }

    // 解析 JSON 请求
    auto body = crow::json::load(req.body);
    if (!body || !body.has("user_id") || !body.has("model_id") || !body.has("address") ||
        !body.has("orderType") || !body.has("paymentMethod") || !body.has("totalPrice") ||
        (body["orderType"] == "租赁" && !body.has("rentalDuration"))) {
        result["msg"] = "表单信息错误";
        return crow::response(400, result);
    }

    std::string user_id = body["user_id"].s();
    std::string model_id = body["model_id"].s();
    std::string address = body["address"].s();
    std::string orderType = body["orderType"].s();
    std::string paymentMethod = body["paymentMethod"].s();
    std::string totalPrice = (body["totalPrice"].t() == crow::json::type::Number)
                                 ? std::to_string(body["totalPrice"].d())
                                 : "0.0";
    int rentalDuration =
        (orderType == "租赁" && body["rentalDuration"].t() == crow::json::type::Number)
            ? body["rentalDuration"].i()
            : 0;

    if (user_id != jwt_user_id.value()) {
        result["msg"] = "表单信息错误";
        return crow::response(400, result);
    }

    // todo:使用数据库事务重构
    // 连接数据库
    auto conn = getDatabaseConnection();
    if (!conn) {
        result["msg"] = "数据库连接失败";
        return crow::response(500, result);
    }

    // 防止 SQL 注入
    char safe_user_id[256], safe_model_id[256], safe_address[512], safe_orderType[128],
        safe_paymentMethod[128];

    mysql_real_escape_string(conn.get(), safe_user_id, user_id.c_str(), user_id.length());
    mysql_real_escape_string(conn.get(), safe_model_id, model_id.c_str(), model_id.length());
    mysql_real_escape_string(conn.get(), safe_address, address.c_str(), address.length());
    mysql_real_escape_string(conn.get(), safe_orderType, orderType.c_str(), orderType.length());
    mysql_real_escape_string(conn.get(), safe_paymentMethod, paymentMethod.c_str(),
                             paymentMethod.length());

    // 找到一个可用的电动车
    std::string getVehicleSql = "SELECT vehicle_id FROM vehicles WHERE model_id = '" +
                                std::string(model_id) + "' AND status = '可用' LIMIT 1";
    if (mysql_query(conn.get(), getVehicleSql.c_str()) != 0) {
        result["msg"] = "查询语句错误";
        return crow::response(500, result);
    }

    std::shared_ptr<MYSQL_RES> getVehicleRes(mysql_store_result(conn.get()), mysql_free_result);
    if (!getVehicleRes || mysql_num_rows(getVehicleRes.get()) == 0) {
        result["msg"] = "数据不存在";
        return crow::response(409, result);
    }
    MYSQL_ROW getVehicleRow = mysql_fetch_row(getVehicleRes.get());
    int vehicle_id = std::stoi(getVehicleRow[0]);

    // 修改它的状态
    std::string type = (orderType == "租赁" ? "已租赁" : "已售出");
    std::string updateVehicleSql = "UPDATE vehicles SET status = '" + type +
                                   "' WHERE vehicle_id = " + std::to_string(vehicle_id);
    if (mysql_query(conn.get(), updateVehicleSql.c_str()) != 0) {
        result["msg"] = "修改语句错误";
        return crow::response(500, result);
    }

    // 添加一条订单记录
    std::string order_date = getCurrentDate();
    std::string safe_end_date = addMonthsToDate(order_date, rentalDuration);
    std::string addOrderSql =
        "INSERT INTO orders (user_id, vehicle_id, order_date, order_type, end_date, "
        "total_price, status) "
        "VALUES (" +
        std::string(safe_user_id) + ", " + std::to_string(vehicle_id) + ", '" + order_date +
        "', '" + std::string(safe_orderType) + "', '" + std::string(safe_end_date) + "', " +
        totalPrice + ", '进行中')";

    std::cout << getVehicleSql << std::endl;
    std::cout << updateVehicleSql << std::endl;
    std::cout << addOrderSql << std::endl;

    if (mysql_query(conn.get(), addOrderSql.c_str()) != 0) {
        result["msg"] = "添加语句错误";
        return crow::response(500, result);
    }

    result["msg"] = "订单提交成功";
    return crow::response(200, result);
}