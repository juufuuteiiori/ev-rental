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

double computeModelScore(crow::json::wvalue& vechile) {
    double score = 0;
    //            + car.user_rating * 5.0           // 用户评分
    //            + std::log(car.comments + 1) * 1.2  // 评论数

    //    std::log(car.orders + 1) * 2.0   // 订单量
    //            + std::log(car.favorites + 1) * 1.5 // 收藏数
    //            + car.range * 0.01                 // 续航
    //            + car.peak_power * 0.05            // 充电速度
    //            - car.acceleration * 1.0           // 加速时间（越小越好）
    //            + car.seat_count * 0.5             // 座位数
    //            + car.available_count * 0.3        // 可用数量
    //            - car.rental_price * 0.0001        // 租赁价格（越低越好）
    //            - car.sale_price * 0.00001         // 购买价格（越低越好）
    //            + car.is_recommended * 10.0;       // 官方推荐（加分）

    return score;
}
