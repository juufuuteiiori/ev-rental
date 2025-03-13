#include "jwt_utils.h"

#include "jwt-cpp/jwt.h"

const std::string SECRET_KEY = "jwt_secret_key";  // JWT 密钥

// 生成 JWT
std::string generateJWT(const std::string& user_id, const std::string& role) {
    auto token = jwt::create()
                     .set_type("JWT")
                     .set_issuer("ev-rental")
                     .set_subject(user_id)
                     .set_payload_claim("role", jwt::claim(role))
                     .set_issued_at(std::chrono::system_clock::now())
                     .set_expires_at(std::chrono::system_clock::now() + std::chrono::hours(24))
                     .sign(jwt::algorithm::hs256{SECRET_KEY});

    return token;
}

// 验证 JWT
std::optional<std::pair<std::string, std::string>> validateJWT(const std::string& token) {
    try {
        auto decoded = jwt::decode(token);
        auto verifier = jwt::verify()
                            .allow_algorithm(jwt::algorithm::hs256{SECRET_KEY})
                            .with_issuer("ev-rental");

        verifier.verify(decoded);
        std::string user_id = decoded.get_subject();
        std::string role = decoded.get_payload_claim("role").as_string();
        return std::make_pair(user_id, role);
    } catch (...) {
        return std::nullopt;
    }
}
