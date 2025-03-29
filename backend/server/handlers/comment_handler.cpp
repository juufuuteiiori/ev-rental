#include "comment_handler.h"

#include <database/database.h>
#include <server/utils/utils.h>

crow::response getComments(const crow::request& req) {
    crow::json::wvalue result;
    crow::response response;

    auto jwt_result = getJWT(req, response);
    if (!jwt_result) {
        return response;
    }

    auto used_id = jwt_result->first;

    // 连接数据库
    auto conn = getDatabaseConnection();
    if (!conn) {
        result["msg"] = "数据库错误";
        return crow::response(500, result);
    }

    // 查询帖子
    std::string query =
        "SELECT u.user_name, c.content, c.created_at, COUNT(DISTINCT l.like_id) AS like_num, "
        "c.comment_id, "
        "u.user_id, EXISTS ( SELECT 1 FROM likes l WHERE l.comment_id = "
        "c.comment_id AND l.user_id = " +
        std::to_string(used_id) +
        ") AS is_liked, COUNT(DISTINCT d.dislike_id) AS dislike_num, EXISTS ( SELECT 1 FROM "
        "dislikes d "
        "WHERE d.comment_id = "
        "c.comment_id AND d.user_id = " +
        std::to_string(used_id) +
        ") AS is_disliked, COUNT(DISTINCT sub.comment_id) AS reply_num, u.role, "
        "c.recent_activity_date, c.sentiment_score, c.toxicity_score "
        "FROM comments c "
        "JOIN users u ON u.user_id = c.user_id "
        "LEFT JOIN likes l ON c.comment_id = l.comment_id "
        "LEFT JOIN dislikes d ON c.comment_id = d.comment_id "
        "LEFT JOIN comments sub ON c.comment_id = sub.parent_id "
        "WHERE c.parent_id IS NULL "
        "GROUP BY c.comment_id";

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
    std::vector<crow::json::wvalue> comments;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res.get()))) {
        crow::json::wvalue comment;
        comment["author"] = row[0];
        comment["content"] = row[1];
        comment["date"] = row[2];
        comment["like_num"] = std::stoi(row[3]);
        comment["id"] = std::stoi(row[4]);
        comment["user_id"] = std::stoi(row[5]);
        comment["is_liked"] = std::stoi(row[6]) == 1;
        comment["dislike_num"] = std::stoi(row[7]);
        comment["is_disliked"] = std::stoi(row[8]) == 1;
        comment["reply_num"] = std::stoi(row[9]);
        comment["is_admin"] = std::string(row[10]).compare("用户") == 0 ? 0 : 1;
        comment["recent_activity_date"] = row[11];
        comment["sentiment_score"] = std::stoi(row[12]);
        comment["toxicity_score"] = std::stoi(row[13]);

        comments.push_back(std::move(comment));
    }

    sortCommentsByScore(comments);
    result["comments"] = std::move(comments);

    result["msg"] = "成功";
    return crow::response(200, result);
}

crow::response getCommentsById(const crow::request& req) {
    crow::json::wvalue result;
    crow::response response;

    auto param_comment_id = getIntParam(req, response, "comment_id");
    if (!param_comment_id) {
        return response;
    }
    auto comment_id = param_comment_id.value();

    // 连接数据库
    auto conn = getDatabaseConnection();
    if (!conn) {
        result["msg"] = "数据库错误";
        return crow::response(500, result);
    }

    // 查询帖子
    std::string query =
        "SELECT u.user_name, c.content, c.created_at "
        "FROM comments c "
        "JOIN users u ON u.user_id = c.user_id "
        "WHERE c.parent_id = " +
        std::to_string(comment_id) + " ORDER BY c.created_at DESC";

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
    if (!body || !body.has("user_id") || body["user_id"].t() != crow::json::type::Number ||
        !body.has("content") || body["content"].t() != crow::json::type::String) {
        result["msg"] = "表单信息错误";
        return crow::response(400, result);
    }

    auto user_id = body["user_id"].i();
    std::string content = body["content"].s();
    auto parent_id = (body.has("parent_id") && body["parent_id"].t() == crow::json::type::Number)
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

    char safe_content[2048];
    mysql_real_escape_string(conn.get(), safe_content, content.c_str(), content.length());

    auto sentiment_score = 0, toxicity_score = 0;
    if (getSentiment(content, sentiment_score) || getToxicity(content, toxicity_score)) {
        result["msg"] = "NLP服务器连接失败";
        return crow::response(500, result);
    }

    std::string insert_query =
        "INSERT INTO comments(user_id, parent_id, content, sentiment_score, toxicity_score) VALUES "
        "(" +
        std::to_string(user_id) + ", " + (parent_id == 0 ? "NULL" : std::to_string(parent_id)) +
        ", '" + std::string(safe_content) + "', " + std::to_string(sentiment_score) + ", " +
        std::to_string(toxicity_score) + ");";

    if (mysql_query(conn.get(), insert_query.c_str()) != 0) {
        result["msg"] = "数据库错误";
        return crow::response(500, result);
    }

    if (parent_id) {
        std::string refresh_query = "UPDATE comments SET recent_activity_date = '" +
                                    getCurrentDate() +
                                    "' WHERE comment_id = " + std::to_string(parent_id);

        if (mysql_query(conn.get(), refresh_query.c_str()) != 0) {
            result["msg"] = "数据库错误";
            return crow::response(500, result);
        }
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
    auto jwt_user_role = jwt_result->second;

    auto param_comment_id = getIntParam(req, response, "comment_id");
    if (!param_comment_id) {
        return response;
    }
    auto comment_id = param_comment_id.value();

    // 连接数据库
    auto conn = getDatabaseConnection();
    if (!conn) {
        result["msg"] = "数据库连接失败";
        return crow::response(500, result);
    }

    std::string getCommentSql =
        "SELECT user_id FROM comments WHERE comment_id = " + std::to_string(comment_id);

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
    auto user_id = std::stoi(getCommentRow[0]);

    if (user_id != jwt_user_id && jwt_user_role == "用户") {
        result["msg"] = "删帖用户与jwt用户不匹配";
        return crow::response(400, result);
    }

    std::string delCommentSql =
        "DELETE FROM comments WHERE comment_id = " + std::to_string(comment_id);

    if (mysql_query(conn.get(), delCommentSql.c_str()) != 0) {
        result["msg"] = "删除语句错误";
        return crow::response(500, result);
    }

    result["msg"] = "删帖成功";
    return crow::response(200, result);
}

crow::response addLike(const crow::request& req) {
    crow::json::wvalue result;
    crow::response response;

    auto jwt_result = getJWT(req, response);
    if (!jwt_result) {
        return response;
    }

    auto user_id = jwt_result->first;

    auto param_comment_id = getIntParam(req, response, "comment_id");
    if (!param_comment_id) {
        return response;
    }
    auto comment_id = param_comment_id.value();

    // 连接数据库
    auto conn = getDatabaseConnection();
    if (!conn) {
        result["msg"] = "数据库连接失败";
        return crow::response(500, result);
    }

    std::string addLikeSql = "INSERT IGNORE INTO likes (user_id, comment_id) VALUES (" +
                             std::to_string(user_id) + ", " + std::to_string(comment_id) + ");";

    if (mysql_query(conn.get(), addLikeSql.c_str()) != 0) {
        result["msg"] = "插入语句错误";
        return crow::response(500, result);
    }

    std::string refresh_query = "UPDATE comments SET recent_activity_date = '" + getCurrentDate() +
                                "' WHERE comment_id = " + std::to_string(comment_id);

    if (mysql_query(conn.get(), refresh_query.c_str()) != 0) {
        result["msg"] = "数据库错误";
        return crow::response(500, result);
    }

    result["msg"] = "点赞成功";
    return crow::response(200, result);
}

crow::response delLike(const crow::request& req) {
    crow::json::wvalue result;
    crow::response response;

    auto jwt_result = getJWT(req, response);
    if (!jwt_result) {
        return response;
    }

    auto user_id = jwt_result->first;

    auto param_comment_id = getIntParam(req, response, "comment_id");
    if (!param_comment_id) {
        return response;
    }
    auto comment_id = param_comment_id.value();

    // 连接数据库
    auto conn = getDatabaseConnection();
    if (!conn) {
        result["msg"] = "数据库连接失败";
        return crow::response(500, result);
    }

    std::string delLikeSql = "DELETE FROM likes WHERE user_id = " + std::to_string(user_id) +
                             " AND comment_id = " + std::to_string(comment_id);

    if (mysql_query(conn.get(), delLikeSql.c_str()) != 0) {
        result["msg"] = "删除语句错误";
        return crow::response(500, result);
    }

    result["msg"] = "取消点赞成功";
    return crow::response(200, result);
}

crow::response addDislike(const crow::request& req) {
    crow::json::wvalue result;
    crow::response response;

    auto jwt_result = getJWT(req, response);
    if (!jwt_result) {
        return response;
    }

    auto user_id = jwt_result->first;

    auto param_comment_id = getIntParam(req, response, "comment_id");
    if (!param_comment_id) {
        return response;
    }
    auto comment_id = param_comment_id.value();

    // 连接数据库
    auto conn = getDatabaseConnection();
    if (!conn) {
        result["msg"] = "数据库连接失败";
        return crow::response(500, result);
    }

    std::string addLikeSql = "INSERT IGNORE INTO dislikes (user_id, comment_id) VALUES (" +
                             std::to_string(user_id) + ", " + std::to_string(comment_id) + ");";

    if (mysql_query(conn.get(), addLikeSql.c_str()) != 0) {
        result["msg"] = "插入语句错误";
        return crow::response(500, result);
    }

    std::string refresh_query = "UPDATE comments SET recent_activity_date = '" + getCurrentDate() +
                                "' WHERE comment_id = " + std::to_string(comment_id);

    if (mysql_query(conn.get(), refresh_query.c_str()) != 0) {
        result["msg"] = "数据库错误";
        return crow::response(500, result);
    }

    result["msg"] = "添加成功";
    return crow::response(200, result);
}

crow::response delDislike(const crow::request& req) {
    crow::json::wvalue result;
    crow::response response;

    auto jwt_result = getJWT(req, response);
    if (!jwt_result) {
        return response;
    }

    auto user_id = jwt_result->first;

    auto param_comment_id = getIntParam(req, response, "comment_id");
    if (!param_comment_id) {
        return response;
    }
    auto comment_id = param_comment_id.value();

    // 连接数据库
    auto conn = getDatabaseConnection();
    if (!conn) {
        result["msg"] = "数据库连接失败";
        return crow::response(500, result);
    }

    std::string delLikeSql = "DELETE FROM dislikes WHERE user_id = " + std::to_string(user_id) +
                             " AND comment_id = " + std::to_string(comment_id);

    if (mysql_query(conn.get(), delLikeSql.c_str()) != 0) {
        result["msg"] = "删除语句错误";
        return crow::response(500, result);
    }

    result["msg"] = "取消成功";
    return crow::response(200, result);
}