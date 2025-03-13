#ifndef JWT_UTILS_H
#define JWT_UTILS_H

#include <optional>
#include <string>

std::string generateJWT(const std::string& user_id, const std::string& role);
std::optional<std::pair<std::string, std::string>> validateJWT(const std::string& token);

#endif  // JWT_UTILS_H
