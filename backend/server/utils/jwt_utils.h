#ifndef JWT_UTILS_H
#define JWT_UTILS_H

#include <optional>
#include <string>

std::string generateJWT(const std::string& user_id);
std::optional<std::string> validateJWT(const std::string& token);

#endif  // JWT_UTILS_H
