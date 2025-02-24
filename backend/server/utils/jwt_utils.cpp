#include "jwt_utils.h"

#include "jwt-cpp/jwt.h"

const std::string SECRET_KEY = "jwt_secret_key";  // JWT 密钥

// 生成 JWT
std::string generateJWT(const std::string& user_id) {
    auto token = jwt::create()
                     .set_type("JWT")
                     .set_issuer("ev-rental")
                     .set_subject(user_id)
                     .set_issued_at(std::chrono::system_clock::now())
                     .set_expires_at(std::chrono::system_clock::now() + std::chrono::hours(24))
                     .sign(jwt::algorithm::hs256{SECRET_KEY});

    return token;
}

// 验证 JWT
std::optional<std::string> validateJWT(const std::string& token) {
    try {
        auto decoded = jwt::decode(token);
        auto verifier = jwt::verify()
                            .allow_algorithm(jwt::algorithm::hs256{SECRET_KEY})
                            .with_issuer("ev-rental");

        verifier.verify(decoded);
        return decoded.get_subject();  // 返回 user_id
    } catch (...) {
        return std::nullopt;
    }
}
