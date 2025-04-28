#include "recommendation_utils.h"

#include <server/utils/time_utils.h>

double computeCommentScore(crow::json::wvalue& comment) {
    double score = 0;

    score += std::log(std::stoi(comment["like_num"].dump()) + 1.0) * 3.0;     // 点赞次数
    score += std::log(std::stoi(comment["reply_num"].dump()) + 1.0) * 4.0;    // 回复数量
    score -= std::log(std::stoi(comment["dislike_num"].dump()) + 1.0) * 2.0;  // 不喜欢数量
    score -= std::log(getDiffDay(comment["recent_activity_date"].dump()) + 1.0) * 3.0;  // 是否活跃

    score += std::stoi(comment["is_admin"].dump()) * 10.0;  //管理员帖子

    score += std::stoi(comment["sentiment_score"].dump()) * 2.0;
    if (std::stoi(comment["sentiment_score"].dump()) < 0) {  // 确保高质量的批评不会被埋没
        score += std::log(std::stoi(comment["reply_num"].dump()) + 1.0) * 2.0;
    }

    score -= std::stoi(comment["toxicity_score"].dump()) * 15.0;  // 冒犯性言论

    return score;
}

void sortCommentsByScore(std::vector<crow::json::wvalue>& comments) {
    std::sort(comments.begin(), comments.end(),
              [](auto& a, auto& b) { return computeCommentScore(a) > computeCommentScore(b); });
}

double computeVehicleScore(crow::json::wvalue& vehicle) {
    double score = 0;

    score += std::sqrt(std::stoi(vehicle["range"].dump())) * 0.05;  // 续航
    score -= std::log(std::stod(vehicle["rentalPrice"].dump()) + 1) * 0.05;  // 租赁价格（越低越好）
    score -= std::log(std::stod(vehicle["salePrice"].dump()) + 1) * 0.02;  // 购买价格（越低越好）
    score += std::stoi(vehicle["peak_power"].dump()) * 0.05;               // 充电速度
    score -=
        std::pow(std::stod(vehicle["acceleration"].dump()), 1.5) * 0.8;  // 加速时间（越小越好）
    score += std::stoi(vehicle["seat_count"].dump()) * 0.5;              // 座位数
    score += std::sqrt(std::stoi(vehicle["storage_space"].dump())) * 0.5;  // 存储空间
    score += std::stoi(vehicle["is_star"].dump()) * 5.00;                  // 收藏
    score += std::stoi(vehicle["available_number"].dump()) * 0.3;          // 可用数量
    score += std::stoi(vehicle["is_recommend"].dump()) * 12.0;  // 官方推荐（加分）
    score += std::log(std::stoi(vehicle["order_count"].dump()) + 1) * 2.5;  // 订单量

    int review_count = std::stoi(vehicle["review_count"].dump());
    score += std::log(review_count + 1) * 1.2;  // 评论数

    if (vehicle["avg_rating"].dump() != "null") {
        double rating = std::stod(vehicle["avg_rating"].dump());

        // 评分数量的权重（平方根+平滑）
        double weight = std::sqrt(review_count + 1);

        // 评分影响：以 3 分为基准，高于 3 分加分，低于 3 分扣分
        score += (rating - 3) * 3.0 * weight;
    }

    return score;
}

void sortVehiclesByScore(std::vector<crow::json::wvalue>& vehicles) {
    std::sort(vehicles.begin(), vehicles.end(),
              [](auto& a, auto& b) { return computeVehicleScore(a) > computeVehicleScore(b); });
}