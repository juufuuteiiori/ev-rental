#include "user_handler.h"

#include <database/database.h>
#include <server/utils/jwt_utils.h>

#include <bcrypt/BCrypt.hpp>

crow::response registerUser(const crow::request& req) {
    crow::json::wvalue result;

    // 解析 JSON 请求
    auto body = crow::json::load(req.body);
    if (!body || !body.has("user_name") || !body.has("user_password")) {
        result["code"] = 0;
        result["msg"] = "缺少用户名或密码";
        return crow::response(400, result);
    }

    std::string user_name = body["user_name"].s();
    std::string user_password = body["user_password"].s();
    std::string hashed_password = BCrypt::generateHash(user_password);  // 哈希密码

    // 连接数据库
    auto conn = getDatabaseConnection();
    if (!conn) {
        result["code"] = 0;
        result["msg"] = "数据库错误";
        return crow::response(500, result);
    }

    // 防止 SQL 注入
    char safe_name[100], safe_password[255];
    mysql_real_escape_string(conn.get(), safe_name, user_name.c_str(), user_name.length());
    mysql_real_escape_string(conn.get(), safe_password, hashed_password.c_str(),
                             hashed_password.length());

    std::string check_query =
        "SELECT user_id FROM users WHERE user_name = '" + std::string(safe_name) + "'";

    if (mysql_query(conn.get(), check_query.c_str()) != 0) {
        result["code"] = 0;
        result["msg"] = "数据库错误";
        return crow::response(500, result);
    }

    MYSQL_RES* res = mysql_store_result(conn.get());
    if (res && mysql_num_rows(res) > 0) {
        mysql_free_result(res);
        result["code"] = 0;
        result["msg"] = "用户名已存在";
        return crow::response(409, result);
    }
    mysql_free_result(res);

    std::string insert_query = "INSERT INTO users (user_name, user_password) VALUES ('" +
                               std::string(safe_name) + "', '" + std::string(safe_password) + "')";

    if (mysql_query(conn.get(), insert_query.c_str()) != 0) {
        result["code"] = 0;
        result["msg"] = "数据库错误";
        return crow::response(500, result);
    }

    int user_id = mysql_insert_id(conn.get());
    std::string token = generateJWT(user_name);

    result["code"] = 1;
    result["msg"] = "注册成功";
    result["data"]["user_id"] = std::to_string(user_id);
    result["data"]["token"] = token;
    return crow::response(200, result);
}

crow::response loginUser(const crow::request& req) {
    crow::json::wvalue result;

    // 解析 JSON 请求
    auto body = crow::json::load(req.body);
    if (!body || !body.has("user_name") || !body.has("user_password")) {
        result["code"] = 0;
        result["msg"] = "缺少用户名或密码";
        return crow::response(400, result);
    }

    std::string user_name = body["user_name"].s();
    std::string user_password = body["user_password"].s();

    // 连接数据库
    auto conn = getDatabaseConnection();
    if (!conn) {
        result["code"] = 0;
        result["msg"] = "数据库错误";
        return crow::response(500, result);
    }

    // 防止 SQL 注入
    char safe_name[100];
    mysql_real_escape_string(conn.get(), safe_name, user_name.c_str(), user_name.length());

    // 查询数据库，获取存储的哈希密码
    std::string check_query = "SELECT user_id, user_password FROM users WHERE user_name = '" +
                              std::string(safe_name) + "'";

    if (mysql_query(conn.get(), check_query.c_str()) != 0) {
        result["code"] = 0;
        result["msg"] = "数据库错误";
        return crow::response(500, result);
    }

    MYSQL_RES* res = mysql_store_result(conn.get());
    if (!res || mysql_num_rows(res) == 0) {
        if (res) mysql_free_result(res);
        result["code"] = 0;
        result["msg"] = "用户名或密码错误";
        return crow::response(409, result);
    }

    // 获取查询结果
    MYSQL_ROW row = mysql_fetch_row(res);
    std::string user_id = row ? row[0] : "";        // 获取 user_id
    std::string hash_password = row ? row[1] : "";  // 获取哈希密码

    mysql_free_result(res);

    // 验证密码
    if (!BCrypt::validatePassword(user_password, hash_password)) {
        result["code"] = 0;
        result["msg"] = "用户名或密码错误";
        return crow::response(409, result);
    }
    std::string token = generateJWT(user_id);

    result["code"] = 1;
    result["msg"] = "登录成功";
    result["data"]["user_id"] = user_id;
    result["data"]["token"] = token;
    return crow::response(200, result);
}

// BUG：用户只能查自己的信息。管理员需要验证
crow::response getUser(const crow::request& req) {
    crow::json::wvalue result;

    // 获取 `user_id` 参数
    auto user_id = req.url_params.get("user_id");
    if (!user_id) {
        result["code"] = 0;
        result["msg"] = "缺少用户ID";
        return crow::response(400, result);
    }

    // 连接数据库
    auto conn = getDatabaseConnection();
    if (!conn) {
        result["code"] = 0;
        result["msg"] = "数据库错误";
        return crow::response(500, result);
    }

    // 防止 SQL 注入
    char safe_id[10];
    mysql_real_escape_string(conn.get(), safe_id, user_id, strlen(user_id));

    // 查询用户信息
    std::string query =
        "SELECT user_id, user_name, user_phone FROM users WHERE user_id = " + std::string(safe_id);
    if (mysql_query(conn.get(), query.c_str()) != 0) {
        result["code"] = 0;
        result["msg"] = "数据库错误";
        return crow::response(500, result);
    }

    MYSQL_RES* res = mysql_store_result(conn.get());
    MYSQL_ROW row = mysql_fetch_row(res);
    if (!row) {
        mysql_free_result(res);
        result["code"] = 0;
        result["msg"] = "用户ID错误";
        return crow::response(404, result);
    }

    // 组装 JSON 响应
    result["code"] = 1;
    result["msg"] = "查询成功";
    result["data"]["user_id"] = row[0];
    result["data"]["user_name"] = row[1];
    result["data"]["user_phone"] = row[2];

    mysql_free_result(res);
    return crow::response(200, result);
}

crow::response updateUser(const crow::request& req) {
    crow::json::wvalue result;

    // 读取 Authorization 头部
    std::string authorization = req.get_header_value("Authorization");
    if (authorization.empty()) {
        result["code"] = 0;
        result["msg"] = "缺少 Authorization 头部";
        return crow::response(400, result);
    }

    std::string token;
    if (authorization.find("Bearer ") == 0) {
        token = authorization.substr(7);  // 提取 token 部分
    } else {
        result["code"] = 0;
        result["msg"] = "无效的 Authorization 格式";
        return crow::response(400, result);
    }

    // 验证 JWT
    auto jwt_user_id = validateJWT(token);  // 需要实现此函数，验证 token 是否有效
    if (!jwt_user_id) {
        result["code"] = 0;
        result["msg"] = "无效的 token";
        return crow::response(401, result);  // 401 Unauthorized
    }

    // 解析 JSON 请求
    auto body = crow::json::load(req.body);
    if (!body || !body.has("user_name")) {
        result["code"] = 0;
        result["msg"] = "缺少用户名";
        return crow::response(400, result);
    }

    std::string user_id = jwt_user_id.value();
    std::string user_name = body["user_name"].s();

    std::string user_phone =
        body.has("user_phone") ? std::string(body["user_phone"].s()) : std::string();
    std::string user_password =
        body.has("user_password") ? std::string(body["user_password"].s()) : std::string();
    std::string hashed_password =
        !user_password.empty() ? BCrypt::generateHash(user_password) : "";  // 哈希密码

    // 连接数据库
    auto conn = getDatabaseConnection();
    if (!conn) {
        result["code"] = 0;
        result["msg"] = "数据库错误";
        return crow::response(500, result);
    }

    // 防止 SQL 注入
    char safe_id[100], safe_name[100], safe_phone[100], safe_password[255];
    mysql_real_escape_string(conn.get(), safe_id, user_id.c_str(), user_id.length());
    mysql_real_escape_string(conn.get(), safe_name, user_name.c_str(), user_name.length());
    mysql_real_escape_string(conn.get(), safe_phone, user_phone.c_str(), user_phone.length());
    mysql_real_escape_string(conn.get(), safe_password, hashed_password.c_str(),
                             hashed_password.length());

    std::string check_query =
        "SELECT * FROM users WHERE user_name = '" + std::string(safe_name) + "'";
    if (mysql_query(conn.get(), check_query.c_str()) != 0) {
        result["code"] = 0;
        result["msg"] = "数据库错误";
        return crow::response(500, result);
    }
    MYSQL_RES* res = mysql_store_result(conn.get());
    if (res && mysql_num_rows(res) != 0) {
        if (res) mysql_free_result(res);
        result["code"] = 0;
        result["msg"] = "用户名已存在";
        return crow::response(409, result);
    }

    std::string query = "UPDATE users SET user_name = '" + std::string(safe_name) + "'";
    query += ", user_phone = '" + std::string(safe_phone) + "'";
    if (!user_password.empty()) {
        query += ", user_password = '" + std::string(safe_password) + "'";
    }
    query += " WHERE user_id = '" + std::string(safe_id) + "'";

    if (mysql_query(conn.get(), query.c_str()) != 0) {
        result["code"] = 0;
        result["msg"] = "数据库错误";
        return crow::response(500, result);
    }

    result["code"] = 1;
    result["msg"] = "修改成功";
    return crow::response(200, result);
}