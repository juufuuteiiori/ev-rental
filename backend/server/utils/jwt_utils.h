#ifndef JWT_UTILS_H
#define JWT_UTILS_H

#include <crow.h>

#include <optional>
#include <string>

std::string generateJWT(const int user_id, const std::string& role);
std::optional<std::pair<int, std::string>> validateJWT(const std::string& token);
std::optional<std::pair<int, std::string>> getJWT(const crow::request& req,
                                                  crow::response& response);

#endif  // JWT_UTILS_H