#ifndef RECOMMENDATION_UTILS_H
#define RECOMMENDATION_UTILS_H

#include <crow.h>

void sortCommentsByScore(std::vector<crow::json::wvalue>& comments);
void sortVehiclesByScore(std::vector<crow::json::wvalue>& vehicles);

#endif  // RECOMMENDATION_UTILS_H