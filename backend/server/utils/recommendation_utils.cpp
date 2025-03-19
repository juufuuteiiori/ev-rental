#include "recommendation_utils.h"

#include <server/utils/time_utils.h>

double computeScore(crow::json::wvalue& comment) {
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
              [](auto& a, auto& b) { return computeScore(a) > computeScore(b); });
}
