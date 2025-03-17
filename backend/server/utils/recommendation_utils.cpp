#include "recommendation_utils.h"

#include <server/utils/time_utils.h>

double computeScore(crow::json::wvalue& comment) {
    double score = 0;

    score += std::log(std::stoi(comment["like_num"].dump()) + 1.0) * 2.0;   // 点赞次数
    score += std::log(std::stoi(comment["reply_num"].dump()) + 1.0) * 5.0;  // 回复数量
    score -= std::stoi(comment["dislike_num"].dump()) * 3.0;                // 不喜欢数量
    score += std::stoi(comment["is_admin"].dump()) * 10.0;                  //管理员帖子
    score -= std::log(getDiffDay(comment["recent_activity_date"].dump()) + 1.0) * 2.0;  // 是否活跃

    // std::cout << comment["id"].dump() << " " << std::stoi(comment["is_admin"].dump()) << " "
    // << comment["is_admin"].dump() << " " << score << std::endl;

    return score;
}

void sortCommentsByScore(std::vector<crow::json::wvalue>& comments) {
    std::sort(comments.begin(), comments.end(),
              [](auto& a, auto& b) { return computeScore(a) > computeScore(b); });
}
