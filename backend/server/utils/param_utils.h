#ifndef PARAM_UTILS_H
#define PARAM_UTILS_H

#include <crow.h>

std::optional<int> getIntParam(const crow::request& req, crow::response& response,
                               const std::string& param_name);

std::optional<std::string> getStringParam(const crow::request& req, crow::response& response,
                                          const std::string& param_name);

#endif  // PARAM_UTILS_H