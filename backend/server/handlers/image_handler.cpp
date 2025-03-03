#include "image_handler.h"

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