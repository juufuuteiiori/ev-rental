#include "comment_handler.h"

#include <database/database.h>
#include <server/utils/jwt_utils.h>

#include <string>

crow::response getComments(const crow::request& req) {
    crow::json::wvalue result;

    // 连接数据库
    auto conn = getDatabaseConnection();
    if (!conn) {
        result["msg"] = "数据库错误";
        return crow::response(500, result);
    }

    // 查询帖子
    std::string query =
        "SELECT u.user_name, c.content, c.created_at, c.like_num, c.comment_id, u.user_id "
        "FROM comments c "
        "JOIN users u ON u.user_id = c.user_id "
        "WHERE c.parent_id IS NULL";

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
    std::vector<crow::json::wvalue> comments;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res.get()))) {
        crow::json::wvalue comment;
        comment["author"] = row[0];
        comment["content"] = row[1];
        comment["date"] = row[2];
        comment["likes"] = row[3];
        comment["id"] = std::stoi(row[4]);
        comment["user_id"] = std::stoi(row[5]);

        comments.push_back(std::move(comment));
    }
    result["comments"] = std::move(comments);

    return crow::response(200, result);
}

crow::response getCommentsById(const crow::request& req) {
    crow::json::wvalue result;

    // 获取 `comment_id` 参数
    auto comment_id = req.url_params.get("comment_id");
    if (!comment_id) {
        result["msg"] = "缺少评论ID";
        return crow::response(400, result);
    }

    // 连接数据库
    auto conn = getDatabaseConnection();
    if (!conn) {
        result["msg"] = "数据库错误";
        return crow::response(500, result);
    }

    // 查询帖子
    std::string query =
        "SELECT u.user_name, c.content, c.created_at, c.like_num "
        "FROM comments c "
        "JOIN users u ON u.user_id = c.user_id "
        "WHERE c.parent_id = " +
        std::string(comment_id);

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
    std::vector<crow::json::wvalue> comments;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res.get()))) {
        crow::json::wvalue comment;
        comment["author"] = row[0];
        comment["content"] = row[1];
        comment["date"] = row[2];

        comments.push_back(std::move(comment));
    }
    result["comments"] = std::move(comments);

    return crow::response(200, result);
}

crow::response addComment(const crow::request& req) {
    crow::json::wvalue result;
    crow::response response;

    auto jwt_result = getJWT(req, response);
    if (!jwt_result) {
        return response;
    }

    auto jwt_user_id = jwt_result->first;

    // 解析 JSON 请求
    auto body = crow::json::load(req.body);
    if (!body || !body.has("user_id") || !body.has("content")) {
        result["msg"] = "表单信息错误";
        return crow::response(400, result);
    }

    std::string user_id = body["user_id"].s();
    std::string content = body["content"].s();
    int parent_id = (body.has("parent_id") && body["parent_id"].t() == crow::json::type::Number)
                        ? body["parent_id"].i()
                        : 0;

    if (user_id != jwt_user_id) {
        result["msg"] = "发帖用户与jwt用户不匹配";
        return crow::response(400, result);
    }

    // 连接数据库
    auto conn = getDatabaseConnection();
    if (!conn) {
        result["msg"] = "数据库连接失败";
        return crow::response(500, result);
    }

    char safe_user_id[256], safe_content[2048];
    mysql_real_escape_string(conn.get(), safe_user_id, user_id.c_str(), user_id.length());
    mysql_real_escape_string(conn.get(), safe_content, content.c_str(), content.length());

    std::string insert_query = "INSERT INTO comments(user_id, parent_id, content) VALUES ('" +
                               std::string(safe_user_id) + "', " +
                               (parent_id == 0 ? "NULL" : std::to_string(parent_id)) + ", '" +
                               std::string(safe_content) + "');";

    if (mysql_query(conn.get(), insert_query.c_str()) != 0) {
        result["msg"] = "数据库错误";
        return crow::response(500, result);
    }

    result["msg"] = "发帖成功";
    return crow::response(200, result);
}

crow::response delComment(const crow::request& req) {
    crow::json::wvalue result;
    crow::response response;

    auto jwt_result = getJWT(req, response);
    if (!jwt_result) {
        return response;
    }

    auto jwt_user_id = jwt_result->first;

    auto comment_id = req.url_params.get("comment_id");
    if (!comment_id) {
        result["msg"] = "缺少订单ID";
        return crow::response(400, result);
    }

    // 连接数据库
    auto conn = getDatabaseConnection();
    if (!conn) {
        result["msg"] = "数据库连接失败";
        return crow::response(500, result);
    }

    // 防止 SQL 注入
    char safe_id[128];
    mysql_real_escape_string(conn.get(), safe_id, comment_id, strlen(comment_id));

    std::string getCommentSql =
        "SELECT user_id FROM comments WHERE comment_id = '" + std::string(safe_id) + "'";

    if (mysql_query(conn.get(), getCommentSql.c_str()) != 0) {
        result["msg"] = "查询语句错误";
        return crow::response(500, result);
    }

    std::shared_ptr<MYSQL_RES> getCommentRes(mysql_store_result(conn.get()), mysql_free_result);
    if (!getCommentRes || mysql_num_rows(getCommentRes.get()) == 0) {
        result["msg"] = "数据不存在";
        return crow::response(409, result);
    }
    MYSQL_ROW getCommentRow = mysql_fetch_row(getCommentRes.get());
    int user_id = std::stoi(getCommentRow[0]);

    if (std::to_string(user_id) != jwt_user_id) {
        result["msg"] = "发帖用户与jwt用户不匹配";
        return crow::response(400, result);
    }

    std::string delCommentSql =
        "DELETE FROM comments WHERE comment_id = '" + std::string(safe_id) + "'";

    if (mysql_query(conn.get(), delCommentSql.c_str()) != 0) {
        result["msg"] = "删除语句错误";
        return crow::response(500, result);
    }

    result["msg"] = "删帖成功";
    return crow::response(200, result);
}