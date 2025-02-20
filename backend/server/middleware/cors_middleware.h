#ifndef CORS_MIDDLEWARE_H
#define CORS_MIDDLEWARE_H

#include "crow.h"

struct CORS_MIDDLEWARE {
    struct context {};  // CROW 的中间件要求提供 context 结构

    void before_handle(crow::request& req, crow::response& res, context&) {
        res.add_header("Access-Control-Allow-Origin", "*");
        res.add_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS, PUT, DELETE");
        res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization");

        // 处理预检请求，直接返回 200
        if (req.method == crow::HTTPMethod::OPTIONS) {
            res.code = 200;
            res.end();
        }
    }

    void after_handle(crow::request& req, crow::response& res, context&) {
        res.set_header("Access-Control-Allow-Origin", "*");
    }
};
#endif  // CORS_MIDDLEWARE_H
