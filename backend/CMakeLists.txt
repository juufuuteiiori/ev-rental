cmake_minimum_required(VERSION 3.10)
project(EV_Rental_Backend)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_WARN_DEPRECATED
    OFF
    CACHE BOOL "" FORCE)
set(CMAKE_BUILD_TYPE Debug) # 设置构建类型为 Debug

add_definitions(-DDBCONFIG_PATH=\"../database/config.json\")
add_definitions(-DASSETS_PATH="${CMAKE_SOURCE_DIR}/assets/images")

# 使用 FetchContent 添加外部依赖
include(FetchContent)

# 添加 Crow
FetchContent_Declare(
    crow
    GIT_REPOSITORY https://github.com/CrowCpp/Crow.git
    GIT_TAG v1.0+3)
FetchContent_MakeAvailable(crow)
include_directories(${crow_SOURCE_DIR}/include)

# 添加 Boost
find_package(Boost REQUIRED COMPONENTS system filesystem)
include_directories(${Boost_INCLUDE_DIRS})

# 添加 nlohmann-json
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.2)
FetchContent_MakeAvailable(json)
include_directories(${json_SOURCE_DIR}/single_include)

# 添加 MySQL 依赖
include_directories("/usr/include/mysql")
link_directories("/usr/lib/x86_64-linux-gnu")

# 添加 OpenSSL 依赖
find_package(OpenSSL REQUIRED)

# 添加 libbcrypt
FetchContent_Declare(
    libbcrypt
    GIT_REPOSITORY https://github.com/trusch/libbcrypt.git
    GIT_TAG master)
FetchContent_MakeAvailable(libbcrypt)
include_directories(${libbcrypt_SOURCE_DIR}/include)

# 添加 jwt-cpp
FetchContent_Declare(
    jwt-cpp
    GIT_REPOSITORY https://github.com/Thalhammer/jwt-cpp.git
    GIT_TAG v0.6.0)
FetchContent_MakeAvailable(jwt-cpp)
include_directories(${jwt-cpp_SOURCE_DIR}/include)

# 添加 include 目录
include_directories(${CMAKE_SOURCE_DIR})

# 编译主程序
add_executable(
    server
    main.cpp
    database/database.cpp
    database/connection_pool.cpp
    server/crow_server.cpp
    server/handlers/vehicle_handler.cpp
    server/handlers/user_handler.cpp
    server/handlers/image_handler.cpp
    server/handlers/order_handler.cpp
    server/handlers/comment_handler.cpp
    server/utils/jwt_utils.cpp
    server/utils/time_utils.cpp
    server/utils/recommendation_utils.cpp)

# 链接所有库
target_link_libraries(server ${Boost_LIBRARIES} mysqlclient bcrypt OpenSSL::Crypto)
