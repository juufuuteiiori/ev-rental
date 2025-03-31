#include "user_handler.h"

#include <database/database.h>
#include <server/utils/utils.h>

#include <bcrypt/BCrypt.hpp>
#include <regex>
#include <string>

#include "crow/json.h"

crow::response registerUser(const crow::request& req) {
    crow::json::wvalue result;

    // 解析 JSON 请求
    auto body = crow::json::load(req.body);
    if (!body || !body.has("user_name") || body["user_name"].t() != crow::json::type::String ||
        !body.has("user_password") || body["user_password"].t() != crow::json::type::String) {
        result["msg"] = "表单错误";
        return crow::response(400, result);
    }

    std::string user_name = body["user_name"].s();
    std::string user_password = BCrypt::generateHash(body["user_password"].s());

    // 连接数据库
    auto conn = getDatabaseConnection();
    if (!conn) {
        result["msg"] = "数据库错误";
        return crow::response(500, result);
    }

    // 防止 SQL 注入
    char safe_name[255], safe_password[255];
    mysql_real_escape_string(conn.get(), safe_name, user_name.c_str(), user_name.length());
    mysql_real_escape_string(conn.get(), safe_password, user_password.c_str(),
                             user_password.length());

    std::string check_query =
        "SELECT user_id FROM users WHERE user_name = '" + std::string(safe_name) + "'";

    if (mysql_query(conn.get(), check_query.c_str()) != 0) {
        result["msg"] = "数据库错误";
        return crow::response(500, result);
    }

    std::shared_ptr<MYSQL_RES> res(mysql_store_result(conn.get()), mysql_free_result);
    if (res && mysql_num_rows(res.get()) > 0) {
        result["msg"] = "用户名已存在";
        return crow::response(409, result);
    }

    std::string insert_query = "INSERT INTO users (user_name, user_password) VALUES ('" +
                               std::string(safe_name) + "', '" + std::string(safe_password) + "')";

    if (mysql_query(conn.get(), insert_query.c_str()) != 0) {
        result["msg"] = "数据库错误";
        return crow::response(500, result);
    }

    int user_id = mysql_insert_id(conn.get());
    std::string token = generateJWT(user_id, "用户");

    result["msg"] = "注册成功";
    result["user_id"] = user_id;
    result["token"] = token;
    return crow::response(200, result);
}

crow::response loginUser(const crow::request& req) {
    crow::json::wvalue result;

    // 解析 JSON 请求
    auto body = crow::json::load(req.body);
    if (!body || !body.has("user_name") || body["user_name"].t() != crow::json::type::String ||
        !body.has("user_password") || body["user_password"].t() != crow::json::type::String) {
        result["msg"] = "表单错误";
        return crow::response(400, result);
    }

    std::string user_name = body["user_name"].s();
    std::string user_password = body["user_password"].s();

    // 连接数据库
    auto conn = getDatabaseConnection();
    if (!conn) {
        result["msg"] = "数据库错误";
        return crow::response(500, result);
    }

    // 防止 SQL 注入
    char safe_name[100];
    mysql_real_escape_string(conn.get(), safe_name, user_name.c_str(), user_name.length());

    // 查询数据库，获取存储的哈希密码
    std::string check_query = "SELECT user_id, user_password, role FROM users WHERE user_name = '" +
                              std::string(safe_name) + "'";

    if (mysql_query(conn.get(), check_query.c_str()) != 0) {
        result["msg"] = "数据库错误";
        return crow::response(500, result);
    }

    std::shared_ptr<MYSQL_RES> res(mysql_store_result(conn.get()), mysql_free_result);
    if (!res || mysql_num_rows(res.get()) == 0) {
        result["msg"] = "用户名或密码错误";
        return crow::response(409, result);
    }

    // 获取查询结果
    MYSQL_ROW row = mysql_fetch_row(res.get());
    int user_id = std::stoi(row[0]);     // 获取 user_id
    std::string hash_password = row[1];  // 获取哈希密码
    std::string role = row[2];           // 权限信息

    // 验证密码
    if (!BCrypt::validatePassword(user_password, hash_password)) {
        result["msg"] = "用户名或密码错误";
        return crow::response(409, result);
    }
    std::string token = generateJWT(user_id, role);

    result["msg"] = "登录成功";
    result["user_id"] = user_id;
    result["token"] = token;
    return crow::response(200, result);
}

crow::response getUser(const crow::request& req) {
    crow::json::wvalue result;
    crow::response response;

    auto jwt_result = getJWT(req, response);
    if (!jwt_result) {
        return response;
    }
    auto jwt_user_id = jwt_result->first;

    auto param_user_id = getIntParam(req, response, "user_id");
    if (!param_user_id) {
        return response;
    }
    auto user_id = param_user_id.value();

    if (user_id != jwt_user_id) {
        result["msg"] = "没有权限访问其他用户信息";
        return crow::response(400, result);
    }

    // 连接数据库
    auto conn = getDatabaseConnection();
    if (!conn) {
        result["msg"] = "数据库错误";
        return crow::response(500, result);
    }

    // 查询用户信息
    std::string query =
        "SELECT user_id, user_name, user_phone, role, user_photo FROM users WHERE user_id = " +
        std::to_string(user_id);
    if (mysql_query(conn.get(), query.c_str()) != 0) {
        result["msg"] = "数据库错误";
        return crow::response(500, result);
    }

    std::shared_ptr<MYSQL_RES> res(mysql_store_result(conn.get()), mysql_free_result);
    MYSQL_ROW row = mysql_fetch_row(res.get());
    if (!row) {
        result["msg"] = "用户ID错误";
        return crow::response(404, result);
    }

    result["msg"] = "查询成功";
    result["user_id"] = std::stoi(row[0]);
    result["user_name"] = row[1];
    result["user_phone"] = row[2] ? row[2] : "";
    result["role"] = row[3];
    result["user_photo"] = row[4] ? row[4] : crow::json::wvalue();
    return crow::response(200, result);
}

crow::response updateUser(const crow::request& req) {
    crow::json::wvalue result;
    crow::response response;

    auto jwt_result = getJWT(req, response);
    if (!jwt_result) {
        return response;
    }

    auto jwt_user_id = jwt_result->first;

    // 解析 JSON 请求
    auto body = crow::json::load(req.body);
    if (!body || !body.has("user_name") || body["user_name"].t() != crow::json::type::String) {
        result["msg"] = "缺少用户名";
        return crow::response(400, result);
    }

    auto user_id = jwt_user_id;
    std::string user_name = body["user_name"].s();

    std::string user_phone =
        (body.has("user_phone") && body["user_phone"].t() == crow::json::type::String)
            ? body["user_phone"].s()
            : std::string();
    std::string user_password =
        (body.has("user_password") && body["user_password"].t() == crow::json::type::String)
            ? BCrypt::generateHash(body["user_password"].s())
            : "";
    std::string user_photo =
        (body.has("user_photo") && body["user_photo"].t() == crow::json::type::String)
            ? body["user_photo"].s()
            : std::string();

    std::regex phone_regex(R"(^1[3-9]\d{9}$)");
    if (!user_phone.empty() && std::regex_match(user_phone, phone_regex) == false) {
        result["msg"] = "手机号码不符合逻辑";
        return crow::response(400, result);
    }

    // 连接数据库
    auto conn = getDatabaseConnection();
    if (!conn) {
        result["msg"] = "数据库错误";
        return crow::response(500, result);
    }

    // 防止 SQL 注入
    char safe_name[100], safe_password[255];
    mysql_real_escape_string(conn.get(), safe_name, user_name.c_str(), user_name.length());
    mysql_real_escape_string(conn.get(), safe_password, user_password.c_str(),
                             user_password.length());

    std::string check_query = "SELECT user_id FROM users WHERE user_name = '" +
                              std::string(safe_name) + "' AND user_id <> " +
                              std::to_string(user_id);

    if (mysql_query(conn.get(), check_query.c_str()) != 0) {
        result["msg"] = "查询语句错误";
        return crow::response(500, result);
    }

    std::shared_ptr<MYSQL_RES> res(mysql_store_result(conn.get()), mysql_free_result);
    if (res && mysql_num_rows(res.get()) != 0) {
        result["msg"] = "用户名已存在";
        return crow::response(409, result);
    }

    std::string query = "UPDATE users SET user_name = '" + std::string(safe_name) + "'";
    if (user_phone.empty())
        query += ", user_phone = NULL";
    else
        query += ", user_phone = '" + user_phone + "'";
    if (!user_password.empty()) {
        query += ", user_password = '" + std::string(safe_password) + "'";
    }
    query += user_photo.empty() ? ", user_photo = NULL" : ", user_photo = '" + user_photo + "'";
    query += " WHERE user_id = " + std::to_string(user_id);

    if (mysql_query(conn.get(), query.c_str()) != 0) {
        result["msg"] = "该手机号已注册";
        return crow::response(500, result);
    }

    result["msg"] = "修改成功";
    return crow::response(200, result);
}

crow::response getAllUser(const crow::request& req) {
    crow::json::wvalue result;
    crow::response response;

    auto jwt_result = getJWT(req, response);
    if (!jwt_result) {
        return response;
    }
    auto jwt_user_role = jwt_result->second;

    if (jwt_user_role != "管理员") {
        result["msg"] = "没有查询权限";
        return crow::response(403, result);
    }

    // 连接数据库
    auto conn = getDatabaseConnection();
    if (!conn) {
        result["msg"] = "数据库错误";
        return crow::response(500, result);
    }

    // 查询所有用户信息
    std::string query =
        "SELECT user_id, user_name, user_phone, user_photo FROM users WHERE role = '用户'";
    if (mysql_query(conn.get(), query.c_str()) != 0) {
        result["msg"] = "数据库错误";
        return crow::response(500, result);
    }

    std::shared_ptr<MYSQL_RES> res(mysql_store_result(conn.get()), mysql_free_result);
    if (!res || mysql_num_rows(res.get()) == 0) {
        result["msg"] = "查询结果为空";
        return crow::response(200, result);
    }

    std::vector<crow::json::wvalue> users;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res.get()))) {
        crow::json::wvalue user;
        user["user_id"] = std::stoi(row[0]);
        user["user_name"] = row[1];
        user["user_phone"] = row[2] ? row[2] : "";
        user["user_photo"] = row[3] ? row[3] : crow::json::wvalue();
        users.push_back(user);
    }

    result["users"] = std::move(users);
    result["msg"] = "查询成功";
    return crow::response(200, result);
}