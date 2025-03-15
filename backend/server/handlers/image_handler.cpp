#include "image_handler.h"

#include <openssl/sha.h>
#include <server/utils/jwt_utils.h>

#include <regex>

std::string urlDecode(const std::string& str) {
    std::ostringstream decoded;
    for (size_t i = 0; i < str.length(); i++) {
        if (str[i] == '%' && i + 2 < str.length()) {
            int hexValue;
            std::istringstream hexStream(str.substr(i + 1, 2));
            if (hexStream >> std::hex >> hexValue) {
                decoded << static_cast<char>(hexValue);
                i += 2;
            } else {
                decoded << '%';  // 解码失败时保留原字符
            }
        } else if (str[i] == '+') {
            decoded << ' ';  // 将 + 转换为空格
        } else {
            decoded << str[i];
        }
    }
    return decoded.str();
}

// 生成 SHA256 Hash
std::string sha256(const std::string& str) {
    // 获取当前时间戳（秒级）
    std::time_t now = std::time(nullptr);
    std::string input = str + std::to_string(now);  // 追加时间戳

    // 计算 SHA256
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)input.c_str(), input.size(), hash);

    std::stringstream ss;
    for (int i = 0; i < 5; i++) {  // 只取前 5 个字符
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}

crow::response getImage(const crow::request& req) {
    std::string raw_path = req.url_params.get("path");
    std::string filename = urlDecode(raw_path);

    // 限制只能访问 images/ 目录下的文件
    if (filename.find("..") != std::string::npos || filename[0] == '/') {
        return crow::response(403, "Forbidden");
    }

    std::string full_path = std::string(ASSETS_PATH) + "/" + filename;
    std::ifstream file(full_path, std::ios::binary);

    if (!file) {
        return crow::response(404, "File not found");
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();
    crow::response result(buffer.str());

    // 设置 Content-Type（动态检测文件类型）
    std::string ext = filename.substr(filename.find_last_of(".") + 1);
    if (ext == "png") {
        result.set_header("Content-Type", "image/png");
    } else if (ext == "jpg" || ext == "jpeg") {
        result.set_header("Content-Type", "image/jpeg");
    } else if (ext == "gif") {
        result.set_header("Content-Type", "image/gif");
    } else {
        result.set_header("Content-Type", "application/octet-stream");
    }

    // 缓存一天
    result.set_header("Cache-Control", "public, max-age=86400");

    return result;
}

crow::response postImage(const crow::request& req) {
    crow::json::wvalue result;

    // 读取 Authorization 头部
    std::string authorization = req.get_header_value("Authorization");
    if (authorization.empty()) {
        result["msg"] = "缺少 Authorization 头部";
        return crow::response(400, result);
    }

    std::string token;
    if (authorization.find("Bearer ") == 0) {
        token = authorization.substr(7);  // 提取 token 部分
    } else {
        result["msg"] = "无效的 Authorization 格式";
        return crow::response(400, result);
    }

    // 验证 JWT
    auto jwt_result = validateJWT(token);
    if (!jwt_result) {
        result["msg"] = "无效的 token";
        return crow::response(401, result);  // 401 Unauthorized
    }

    // 确保请求是 multipart/form-data
    std::string content_type = req.get_header_value("Content-Type");
    if (content_type.find("multipart/form-data") == std::string::npos) {
        result["msg"] = "Invalid file upload";
        return crow::response(400, result);
    }

    std::string body = req.body;

    std::smatch match;
    std::string filename;
    std::regex filename_regex(R"(filename=\"([^\"]+)\")");

    if (std::regex_search(body, match, filename_regex)) {
        filename = match[1].str();  // 提取文件名
        filename = sha256(filename) + "_" + filename;
    }

    // 保存路径
    std::string file_path = std::string(ASSETS_PATH) + "/" + filename;

    // 查找文件内容起始位置
    size_t file_start = body.find("\r\n\r\n", match.position()) + 4;
    size_t file_end = body.rfind("------");  // 多部分结束符
    if (file_start == std::string::npos || file_end == std::string::npos ||
        file_end <= file_start) {
        result["msg"] = "Invalid file data";
        return crow::response(400, result);
    }
    std::string file_data = body.substr(file_start, file_end - file_start);
    std::ofstream out_file(file_path, std::ios::binary);
    if (!out_file) {
        return crow::response(500, "Failed to save file");
    }
    out_file.write(file_data.c_str(), file_data.size());
    out_file.close();

    result["msg"] = "上传成功";
    result["file"] = filename;
    return crow::response(200, result);
}