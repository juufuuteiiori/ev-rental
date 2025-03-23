#include "param_utils.h"

std::optional<int> getIntParam(const crow::request& req, crow::response& response,
                               const std::string& param_name) {
    crow::json::wvalue result;

    std::string param_value = req.url_params.get(param_name) ? req.url_params.get(param_name) : "";

    if (param_value.empty()) {
        result["msg"] = "缺少参数: " + param_name;
        response = crow::response(400, result);
        return std::nullopt;
    }

    try {
        return std::stoi(param_value);
    } catch (const std::exception&) {
        result["msg"] = "参数格式错误，应为整数: " + param_name;
        response = crow::response(400, result);
        return std::nullopt;
    }
}