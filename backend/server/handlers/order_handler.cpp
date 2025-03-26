#include "order_handler.h"

#include <database/database.h>
#include <server/utils/utils.h>

crow::response getOrders(const crow::request& req) {
    crow::json::wvalue result;
    crow::response response;

    auto jwt_result = getJWT(req, response);
    if (!jwt_result) {
        return response;
    }

    auto jwt_user_id = jwt_result->first;
    auto jwt_role = jwt_result->second;

    auto param_user_id = getIntParam(req, response, "user_id");
    if (!param_user_id) {
        return response;
    }
    auto user_id = param_user_id.value();

    if (jwt_user_id != user_id) {
        result["msg"] = "信息错误";
        return crow::response(400, result);
    }

    // 连接数据库
    auto conn = getDatabaseConnection();
    if (!conn) {
        result["msg"] = "数据库错误";
        return crow::response(500, result);
    }

    // 查询订单信息
    std::string query =
        "SELECT o.order_id, o.order_date, m.brand_name, m.model_name, v.license_plate, "
        "o.order_type, o.status, o.user_id "
        "FROM orders o "
        "JOIN vehicles v ON o.vehicle_id = v.vehicle_id "
        "JOIN models m ON v.model_id = m.model_id";
    if (jwt_role == "用户") query += " WHERE o.user_id = " + std::to_string(user_id);

    if (mysql_query(conn.get(), query.c_str()) != 0) {
        result["msg"] = "数据库错误";
        return crow::response(500, result);
    }

    std::shared_ptr<MYSQL_RES> res(mysql_store_result(conn.get()), mysql_free_result);
    if (!res || mysql_num_rows(res.get()) == 0) {
        result["msg"] = "查询结果为空";
        return crow::response(200, result);
    }

    // 遍历结果集
    std::vector<crow::json::wvalue> orders;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res.get()))) {
        crow::json::wvalue order;
        order["order_id"] = std::stoi(row[0]);
        order["order_date"] = row[1];
        order["brand_name"] = row[2];
        order["model_name"] = row[3];
        order["license_plate"] = row[4];
        order["order_type"] = row[5];
        order["order_status"] = row[6];
        order["user_id"] = std::stoi(row[7]);
        orders.push_back(std::move(order));
    }
    result["orders"] = std::move(orders);

    return crow::response(200, result);
}

crow::response getOrderById(const crow::request& req) {
    crow::json::wvalue result;
    crow::response response;

    auto jwt_result = getJWT(req, response);
    if (!jwt_result) {
        return response;
    }

    auto jwt_user_id = jwt_result->first;
    auto jwt_role = jwt_result->second;

    auto param_order_id = getIntParam(req, response, "order_id");
    if (!param_order_id) {
        return response;
    }
    auto order_id = param_order_id.value();

    // 连接数据库
    auto conn = getDatabaseConnection();
    if (!conn) {
        result["msg"] = "数据库错误";
        return crow::response(500, result);
    }

    // 查询订单信息
    std::string query =
        "SELECT o.user_id, o.order_date, m.brand_name, m.model_name, v.license_plate, "
        "o.order_type, o.end_date, o.total_price, o.status, u.user_name, u.user_phone, o.rating, "
        "o.review "
        "FROM orders o "
        "JOIN vehicles v ON o.vehicle_id = v.vehicle_id "
        "JOIN models m ON v.model_id = m.model_id "
        "JOIN users u ON u.user_id = o.user_id "
        "WHERE o.order_id = " +
        std::to_string(order_id);

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

    int user_id = std::stoi(row[0]);
    if (jwt_role == "用户" && jwt_user_id != user_id) {
        result["msg"] = "没有权限访问其他用户订单";
        return crow::response(400, result);
    }

    order["user_id"] = std::stoi(row[0]);
    order["order_date"] = row[1];
    order["brand_name"] = row[2];
    order["model_name"] = row[3];
    order["license_plate"] = row[4];
    order["order_type"] = row[5];
    order["end_date"] = row[6] ? row[6] : crow::json::wvalue();
    order["total_price"] = std::stod(row[7]);
    order["order_status"] = row[8];
    order["user_name"] = row[9];
    order["user_phone"] = row[10];
    order["rating"] = row[11] ? std::stoi(row[11]) : crow::json::wvalue();
    order["review"] = row[12] ? row[12] : crow::json::wvalue();

    result["order"] = std::move(order);

    return crow::response(200, result);
}

crow::response submitOrder(const crow::request& req) {
    crow::json::wvalue result;
    crow::response response;

    auto jwt_result = getJWT(req, response);
    if (!jwt_result) {
        return response;
    }

    auto jwt_user_id = jwt_result->first;
    auto jwt_role = jwt_result->second;

    if (jwt_role == "管理员") {
        result["msg"] = "管理员无法下单";
        return crow::response(401, result);
    }

    // 解析 JSON 请求
    auto body = crow::json::load(req.body);
    if (!body || !body.has("user_id") || body["user_id"].t() != crow::json::type::Number ||
        !body.has("model_id") || body["model_id"].t() != crow::json::type::Number ||
        !body.has("address") || !body.has("orderType") || !body.has("paymentMethod") ||
        !body.has("totalPrice") || body["totalPrice"].t() != crow::json::type::Number ||
        (body["orderType"] == "租赁" &&
         (!body.has("rentalDuration") || body["rentalDuration"].t() != crow::json::type::Number))) {
        result["msg"] = "表单信息错误";
        return crow::response(400, result);
    }

    auto user_id = body["user_id"].i();
    auto model_id = body["model_id"].i();
    std::string address = body["address"].s();
    std::string orderType = body["orderType"].s();
    std::string paymentMethod = body["paymentMethod"].s();
    auto totalPrice = body["totalPrice"].d();
    auto rentalDuration = body["rentalDuration"].i();

    if (user_id != jwt_user_id) {
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
    char safe_address[512], safe_orderType[128], safe_paymentMethod[128];

    mysql_real_escape_string(conn.get(), safe_address, address.c_str(), address.length());
    mysql_real_escape_string(conn.get(), safe_orderType, orderType.c_str(), orderType.length());
    mysql_real_escape_string(conn.get(), safe_paymentMethod, paymentMethod.c_str(),
                             paymentMethod.length());

    // 找到一个可用的电动车
    std::string getVehicleSql =
        "SELECT vehicle_id FROM vehicles WHERE model_id = " + std::to_string(model_id) +
        " AND status = '可用' LIMIT 1";
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
        std::to_string(user_id) + ", " + std::to_string(vehicle_id) + ", '" + order_date + "', '" +
        std::string(safe_orderType) + "', '" + std::string(safe_end_date) + "', " +
        std::to_string(totalPrice) + ", '进行中')";

    if (mysql_query(conn.get(), addOrderSql.c_str()) != 0) {
        result["msg"] = "添加语句错误";
        return crow::response(500, result);
    }

    result["msg"] = "订单提交成功";
    return crow::response(200, result);
}

crow::response orderDone(const crow::request& req) {
    crow::json::wvalue result;
    crow::response response;

    auto jwt_result = getJWT(req, response);
    if (!jwt_result) {
        return response;
    }

    auto jwt_user_id = jwt_result->first;
    auto jwt_role = jwt_result->second;

    auto param_order_id = getIntParam(req, response, "order_id");
    if (!param_order_id) {
        return response;
    }
    auto order_id = param_order_id.value();

    // 连接数据库
    auto conn = getDatabaseConnection();
    if (!conn) {
        result["msg"] = "数据库错误";
        return crow::response(500, result);
    }

    // 查询订单信息
    std::string query =
        "SELECT o.user_id, o.order_type, o.status "
        "FROM orders o "
        "WHERE o.order_id = " +
        std::to_string(order_id);

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

    auto user_id = std::stoi(row[0]);
    if (jwt_role == "用户" && jwt_user_id != user_id) {
        result["msg"] = "没有权限访问其他用户订单";
        return crow::response(400, result);
    }

    auto order_type = row[1];
    auto status = row[2];
    if (std::string(order_type) == "租赁" || std::string(status) == "已完成") {
        result["msg"] = "表单信息有误";
        return crow::response(400, result);
    }

    std::string doneQuery =
        "UPDATE orders SET status = '已完成' WHERE order_id = " + std::to_string(order_id);

    if (mysql_query(conn.get(), doneQuery.c_str()) != 0) {
        result["msg"] = "数据库执行错误";
        return crow::response(500, result);
    }

    result["msg"] = "订单确认成功";
    return crow::response(200, result);
}

crow::response submitComment(const crow::request& req) {
    crow::json::wvalue result;
    crow::response response;

    auto jwt_result = getJWT(req, response);
    if (!jwt_result) {
        return response;
    }

    auto jwt_user_id = jwt_result->first;
    auto jwt_role = jwt_result->second;

    if (jwt_role == "管理员") {
        result["msg"] = "管理员不可修改评价";
        return crow::response(500, result);
    }

    // 解析 JSON 请求
    auto body = crow::json::load(req.body);
    if (!body || !body.has("rating") || body["rating"].t() != crow::json::type::Number ||
        !body.has("review") || body["review"].t() != crow::json::type::String || !body.has("id") ||
        body["id"].t() != crow::json::type::Number) {
        result["msg"] = "表单信息错误";
        return crow::response(400, result);
    }

    auto order_id = body["id"].i();
    auto rating = body["rating"].i();
    std::string review = body["review"].s();

    // 连接数据库
    auto conn = getDatabaseConnection();
    if (!conn) {
        result["msg"] = "数据库连接失败";
        return crow::response(500, result);
    }

    std::string select_query = "SELECT * FROM orders WHERE order_id = " + std::to_string(order_id) +
                               " AND user_id = " + std::to_string(jwt_user_id);

    if (mysql_query(conn.get(), select_query.c_str()) != 0) {
        result["msg"] = "数据库错误";
        return crow::response(500, result);
    }

    std::shared_ptr<MYSQL_RES> select_res(mysql_store_result(conn.get()), mysql_free_result);
    if (!select_res || mysql_num_rows(select_res.get()) == 0) {
        result["msg"] = "用户与订单信息不一致";
        return crow::response(404, result);
    }

    char safe_review[2048];
    mysql_real_escape_string(conn.get(), safe_review, review.c_str(), review.length());

    std::string update_query = "UPDATE orders SET rating = " + std::to_string(rating) +
                               ", review = '" + std::string(safe_review) +
                               "' WHERE order_id = " + std::to_string(order_id);

    if (mysql_query(conn.get(), update_query.c_str()) != 0) {
        result["msg"] = "数据库错误";
        return crow::response(500, result);
    }

    result["msg"] = "修改评价成功";
    return crow::response(200, result);
}