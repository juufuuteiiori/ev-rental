#include "nlp_utils.h"

#include <cpr/cpr.h>

#include <nlohmann/json.hpp>

#define NLP_SERVER_URL "http://localhost:8002"

nlohmann::json callNLPServer(const std::string& endpoint, const std::string& text) {
    // 创建 JSON 请求体
    nlohmann::json requestBody;
    requestBody["text"] = text;

    // 将 JSON 对象序列化为字符串
    std::string jsonRequest = requestBody.dump();

    // 发送 HTTP POST 请求
    std::string url = std::string(NLP_SERVER_URL) + "/" + endpoint;
    cpr::Response response = cpr::Post(
        cpr::Url{url}, cpr::Header{{"Content-Type", "application/json"}}, cpr::Body{jsonRequest});

    if (response.status_code == 200) {
        return nlohmann::json::parse(response.text);  // 直接返回 JSON 响应
    } else {
        return nlohmann::json{{"error", "Failed to reach NLP server"}};
    }
}

std::map<std::string, int> sentiment_score_map = {
    {"Very Positive", 2}, {"Positive", 1}, {"Neutral", 0}, {"Negative", -1}, {"Very Negative", -2}};

// 情感分析
int getSentiment(const std::string& str, int& result) {
    auto body = callNLPServer("sentiment", str);
    if (!body.contains("label") || !body.contains("score")) return -1;
    if (body["score"] > 0.3)
        result = sentiment_score_map[body["label"]];
    else
        result = 0;
    return 0;
}

std::map<std::string, int> toxicity_score_map = {{"toxic", 1}, {"neutral", 0}};

// 冒犯性言论检测
int getToxicity(const std::string& str, int& result) {
    auto body = callNLPServer("toxicity", str);
    if (!body.contains("label")) return -1;
    if (body["score"] > 0.3)
        result = toxicity_score_map[body["label"]];
    else
        result = 0;
    return 0;
}