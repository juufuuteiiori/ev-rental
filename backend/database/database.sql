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
    license_plate VARCHAR(20) NOT NULL UNIQUE,                 -- 车牌号
    purchase_date DATE,                                        -- 购置日期
    location VARCHAR(100),                                     -- 当前所在位置（城市/区域）
    status ENUM('可用', '维修中', '不可用') NOT NULL             -- 电动车状态
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
    image_paths TEXT                                           -- 图片路径（多张图片，用逗号分隔）
);

INSERT INTO models (brand_name, model_name, power_type, max_range, leasing_price, purchase_price, peak_power, acceleration, seat_count, storage_space, image_paths)
SELECT * FROM (
    SELECT '特斯拉', 'Model 3', '电动', 600, 4500.00, 280000.00, 250.00, 3.5, 5, 425, '' UNION ALL
    SELECT '比亚迪', '汉 EV', '电动', 605, 3800.00, 250000.00, 200.00, 3.9, 5, 500, '' UNION ALL
    SELECT '蔚来', 'ET7', '电动', 700, 5000.00, 450000.00, 280.00, 3.8, 5, 520, '' UNION ALL
    SELECT '小鹏', 'P7', '电动', 670, 4000.00, 270000.00, 220.00, 4.0, 5, 450, '' UNION ALL
    SELECT '理想', 'L9', '混动', 1100, 6000.00, 460000.00, 180.00, 5.5, 6, 550, '' UNION ALL
    SELECT '极氪', '001', '电动', 620, 4300.00, 320000.00, 240.00, 3.8, 5, 500, '' UNION ALL
    SELECT '哪吒', 'S', '电动', 650, 3500.00, 210000.00, 210.00, 4.2, 5, 400, '' UNION ALL
    SELECT '广汽埃安', 'AION Y', '电动', 510, 3000.00, 160000.00, 150.00, 6.9, 5, 380, '' UNION ALL
    SELECT '宝马', 'iX3', '电动', 550, 5500.00, 400000.00, 200.00, 5.0, 5, 500, '' UNION ALL
    SELECT '奔驰', 'EQE', '电动', 630, 6000.00, 480000.00, 260.00, 4.5, 5, 540, ''
) AS tmp
WHERE NOT EXISTS (SELECT 1 FROM models);

-- 车辆型号关系表
CREATE TABLE IF NOT EXISTS vehicle_model (
    relation_id INT PRIMARY KEY AUTO_INCREMENT,                -- 关系记录的唯一标识
    vehicle_id INT NOT NULL,                                   -- 外键：电动车表的唯一标识
    model_id INT NOT NULL,                                     -- 外键：型号表的唯一标识
    FOREIGN KEY (vehicle_id) REFERENCES vehicles(vehicle_id),  -- 关联电动车表
    FOREIGN KEY (model_id) REFERENCES models(model_id)         -- 关联型号表
);
