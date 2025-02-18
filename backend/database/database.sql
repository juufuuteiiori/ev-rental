CREATE DATABASE IF NOT EXISTS ev_rental_database CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;

USE ev_rental_database;

-- 用户表
CREATE TABLE IF NOT EXISTS users (
    user_id INT NOT NULL AUTO_INCREMENT PRIMARY KEY,           -- 用户的唯一标识
    user_name VARCHAR(50) NOT NULL,                            -- 用户名
    user_phone VARCHAR(15) NOT NULL,                           -- 用户的手机号
    user_password VARCHAR(255) NOT NULL                        -- 用户的登录密码
);

-- 电动车表
CREATE TABLE IF NOT EXISTS vehicles (
    vehicle_id INT PRIMARY KEY AUTO_INCREMENT,                 -- 电动车唯一标识
    status ENUM('可用', '维修中', '不可用') NOT NULL,           -- 电动车状态
    license_plate VARCHAR(20) NOT NULL UNIQUE,                 -- 车牌号
    purchase_date DATE,                                        -- 购置日期
    location VARCHAR(100),                                     -- 当前所在位置（城市/区域）
    last_service_date DATE                                     -- 最近一次维修日期
);

-- 订单表
CREATE TABLE IF NOT EXISTS orders (
    order_id INT NOT NULL AUTO_INCREMENT PRIMARY KEY,          -- 订单唯一标识
    user_id INT NOT NULL,                                      -- 外键，关联用户表，标识下单用户
    vehicle_id INT NOT NULL,                                   -- 外键，关联电动车表，标识租赁车辆
    order_date DATETIME NOT NULL,                              -- 订单创建时间
    order_type ENUM('购买', '租赁') NOT NULL,                   -- 订单类型
    start_date DATE DEFAULT NULL,                              -- 租赁开始日期（可空）
    end_date DATE DEFAULT NULL,                                -- 租赁结束日期（可空）
    total_price DECIMAL(10, 2) NOT NULL,                       -- 订单总金额（单位：元）
    FOREIGN KEY (user_id) REFERENCES users(user_id),           -- 外键，关联用户表
    FOREIGN KEY (vehicle_id) REFERENCES vehicles(vehicle_id)   -- 外键，关联电动车表
);

-- 型号表
CREATE TABLE IF NOT EXISTS models (
    model_id INT NOT NULL AUTO_INCREMENT PRIMARY KEY,          -- 型号唯一标识
    brand_name VARCHAR(100) NOT NULL,                          -- 品牌名称
    model_name VARCHAR(100) NOT NULL,                          -- 型号名称
    power_type ENUM('电动', '混动') NOT NULL,                   -- 动力类型
    max_range INT,                                             -- 最大续航里程（单位：公里）
    leasing_price DECIMAL(10, 2) NOT NULL,                     -- 租赁价格（每月，单位：元）
    purchase_price DECIMAL(10, 2) NOT NULL,                    -- 购买价格（单位：元）
    peak_power DECIMAL(5, 2),                                  -- 充电峰值功率（单位：kW）
    acceleration DECIMAL(5, 2),                                -- 加速度（百公里，单位：秒）
    seat_count INT,                                            -- 座位数
    storage_space INT,                                         -- 储物空间（单位：升）
    image_paths TEXT,                                          -- 图片路径（多张图片，用逗号分隔）
    added_date DATETIME DEFAULT CURRENT_TIMESTAMP              -- 添加日期
);

-- 车辆型号关系表
CREATE TABLE IF NOT EXISTS vehicle_model (
    relation_id INT PRIMARY KEY AUTO_INCREMENT,                -- 关系记录的唯一标识
    vehicle_id INT NOT NULL,                                   -- 外键：电动车表的唯一标识
    model_id INT NOT NULL,                                     -- 外键：型号表的唯一标识
    FOREIGN KEY (vehicle_id) REFERENCES vehicles(vehicle_id),  -- 关联电动车表
    FOREIGN KEY (model_id) REFERENCES models(model_id)         -- 关联型号表
);
