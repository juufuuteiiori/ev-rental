USE ev_rental_database;

-- 用户表
CREATE TABLE IF NOT EXISTS users (
    user_id INT NOT NULL AUTO_INCREMENT PRIMARY KEY,           -- 用户的唯一标识
    user_name VARCHAR(50) NOT NULL UNIQUE,                     -- 用户名
    user_phone VARCHAR(15) UNIQUE,                             -- 用户的手机号
    user_password VARCHAR(255) NOT NULL,                       -- 用户的登录密码
    role ENUM('用户', '管理员') NOT NULL DEFAULT '用户'         -- 权限管理 
);

INSERT INTO users (user_name, user_phone, user_password, role)
VALUES
    ('alice', '13800000001', '$2a$12$Hl5x.7DMV672OSyhatWmb.46Ei.zfzXjNu1JZ60LStBw7yIB0xING', '用户'),
    ('bob', '13800000002', '$2a$12$Df5rFcexYWgfqLO3Yk2laOi16lHYHW3AVnGT/ECJU6AWGX8.Bip6C', '用户'),
    ('charlie', '13800000003', '$2a$12$/G3SDM0skF0nbDWUEmYjW.VDcAuM5Eld700nxz/tLjCQZVXSjJI8u', '用户'),
    ('david', '13800000004', '$2a$12$qeTR9nkR.2GRBQH0z9ubUOimoAag.StU25/wNmtqeRK1bzyCxYqti', '用户'),
    ('eve', '13800000005', '$2a$12$Qg63lFuLfmaMtCBSHDWZJ.euD07RUOPggxE1Oh0.TqvsEtbo4.tOq', '用户'),
    ('iori', '13800000006', '$2a$12$Qg63lFuLfmaMtCBSHDWZJ.euD07RUOPggxE1Oh0.TqvsEtbo4.tOq', '管理员')
ON DUPLICATE KEY UPDATE 
    user_phone = VALUES(user_phone), 
    user_password = VALUES(user_password);  -- 默认密码为 "password"

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
    recommend BOOLEAN NOT NULL DEFAULT FALSE,                  -- 是否为推荐车辆
    UNIQUE KEY unique_brand_model (brand_name, model_name)     -- 确保同一车型同一天只能有一条记录
);

INSERT INTO models (brand_name, model_name, power_type, max_range, leasing_price, purchase_price, peak_power, acceleration, seat_count, storage_space, image_paths, recommend)
VALUES
    ('特斯拉', 'Model 3', '电动', 600, 4500.00, 280000.00, 250.00, 3.5, 5, 425, 'byd_sl.jpg,byd_sl.jpg', TRUE),
    ('比亚迪', '汉 EV', '电动', 605, 3800.00, 250000.00, 200.00, 3.9, 5, 500, 'byd_sl.jpg,byd_sl.jpg', TRUE),
    ('蔚来', 'ET7', '电动', 700, 5000.00, 450000.00, 280.00, 3.8, 5, 520, 'byd_sl.jpg', FALSE),
    ('小鹏', 'P7', '电动', 670, 4000.00, 270000.00, 220.00, 4.0, 5, 450, 'byd_sl.jpg', FALSE),
    ('理想', 'L9', '混动', 1100, 6000.00, 460000.00, 180.00, 5.5, 6, 550, 'byd_sl.jpg', TRUE),
    ('极氪', '001', '电动', 620, 4300.00, 320000.00, 240.00, 3.8, 5, 500, 'byd_sl.jpg', FALSE),
    ('哪吒', 'S', '电动', 650, 3500.00, 210000.00, 210.00, 4.2, 5, 400, 'byd_sl.jpg', FALSE),
    ('广汽埃安', 'AION Y', '电动', 510, 3000.00, 160000.00, 150.00, 6.9, 5, 380, 'byd_sl.jpg', FALSE),
    ('宝马', 'iX3', '电动', 550, 5500.00, 400000.00, 200.00, 5.0, 5, 500, 'byd_sl.jpg', TRUE),
    ('奔驰', 'EQE', '电动', 630, 6000.00, 480000.00, 260.00, 4.5, 5, 540, 'byd_sl.jpg', TRUE)
ON DUPLICATE KEY UPDATE
    power_type = VALUES(power_type),
    max_range = VALUES(max_range),
    leasing_price = VALUES(leasing_price),
    purchase_price = VALUES(purchase_price),
    peak_power = VALUES(peak_power),
    acceleration = VALUES(acceleration),
    seat_count = VALUES(seat_count),
    storage_space = VALUES(storage_space),
    image_paths = VALUES(image_paths),
    recommend = VALUES(recommend);

-- 电动车表
CREATE TABLE IF NOT EXISTS vehicles (
    vehicle_id INT PRIMARY KEY AUTO_INCREMENT,                 -- 电动车唯一标识
    model_id INT NOT NULL,                                     -- 关联的车型 ID
    license_plate VARCHAR(20) NOT NULL UNIQUE,                 -- 车牌号
    purchase_date DATE,                                        -- 购置日期
    location VARCHAR(100),                                     -- 当前所在位置（城市/区域）
    status ENUM('可用', '已租赁', '已售出', '不可用') NOT NULL,  -- 电动车状态
    FOREIGN KEY (model_id) REFERENCES models(model_id) ON DELETE CASCADE
);

INSERT INTO vehicles (model_id, license_plate, purchase_date, location, status)
VALUES
    -- 特斯拉 Model 3
    (1, '川A10001', '2023-06-15', '四川成都 天府新区', '可用'),
    (1, '川A10002', '2023-07-20', '四川成都 高新区', '已租赁'),
    (1, '川A10003', '2023-08-05', '四川成都 锦江区', '可用'),

    -- 比亚迪 汉 EV
    (2, '川A10004', '2023-05-10', '四川成都 武侯区', '可用'),
    (2, '川A10005', '2023-06-25', '四川成都 成华区', '已租赁'),
    (2, '川A10006', '2023-07-30', '四川成都 金牛区', '可用'),

    -- 蔚来 ET7
    (3, '川A10007', '2023-09-10', '四川成都 青羊区', '已售出'),
    (3, '川A10008', '2023-10-15', '四川成都 双流区', '不可用'),

    -- 小鹏 P7
    (4, '川A10009', '2023-11-05', '四川成都 新都区', '已售出'),
    (4, '川A10010', '2023-12-12', '四川成都 温江区', '可用'),

    -- 理想 L9
    (5, '川A10011', '2023-06-07', '四川成都 郫都区', '可用'),
    (5, '川A10012', '2023-07-08', '四川成都 龙泉驿区', '已租赁'),

    -- 极氪 001
    (6, '川A10013', '2023-08-09', '四川成都 青白江区', '可用'),
    (6, '川A10014', '2023-09-20', '四川成都 都江堰市', '已售出'),

    -- 哪吒 S
    (7, '川A10015', '2023-04-18', '四川成都 彭州市', '不可用'),

    -- 广汽埃安 AION Y
    (8, '川A10016', '2023-05-29', '四川成都 崇州市', '已租赁'),

    -- 宝马 iX3
    (9, '川A10017', '2023-06-30', '四川成都 邛崃市', '可用'),

    -- 奔驰 EQE
    (10, '川A10018', '2023-07-25', '四川成都 金堂县', '已售出')
ON DUPLICATE KEY UPDATE
    purchase_date = VALUES(purchase_date),
    location = VALUES(location),
    status = VALUES(status);

CREATE TABLE IF NOT EXISTS orders (
    order_id INT NOT NULL AUTO_INCREMENT PRIMARY KEY,          -- 订单唯一标识
    user_id INT NOT NULL,                                      -- 关联用户表，标识下单用户
    vehicle_id INT NOT NULL,                                   -- 关联电动车表，标识租赁车辆
    order_date DATE NOT NULL,             -- 订单创建日期
    order_type ENUM('购买', '租赁') NOT NULL,                   -- 订单类型
    end_date DATE DEFAULT NULL,                                -- 租赁结束日期（仅租赁适用）
    total_price DECIMAL(10, 2) NOT NULL,                       -- 订单总金额（单位：元）
    status ENUM('进行中', '已完成') NOT NULL DEFAULT '进行中',   -- 订单状态

    CHECK (order_type = '购买' OR (end_date IS NOT NULL AND end_date >= order_date)), -- 确保租赁订单有日期
    FOREIGN KEY (user_id) REFERENCES users(user_id) ON DELETE CASCADE, -- 关联用户，用户删除时订单删除
    FOREIGN KEY (vehicle_id) REFERENCES vehicles(vehicle_id) ON DELETE CASCADE, -- 关联车辆，车辆删除时订单删除
    CONSTRAINT unique_user_vehicle_order UNIQUE (user_id, vehicle_id, order_date)
);

INSERT INTO orders (user_id, vehicle_id, order_date, order_type, end_date, total_price, status)
VALUES
    (1, 1, '2024-06-15', '租赁', '2024-12-15', 27000.00, '已完成'),
    (5, 12, '2024-07-08', '租赁', '2025-03-08', 48000.00, '进行中'),
    (4, 4, '2024-07-15', '租赁', '2024-10-15', 12000.00, '已完成'),
    (3, 3, '2024-08-01', '租赁', '2024-12-01', 18000.00, '已完成'),
    (3, 7, '2024-09-10', '购买', NULL, 450000.00, '已完成'),
    (5, 14, '2024-09-20', '购买', NULL, 320000.00, '已完成'),
    (4, 9, '2024-11-05', '购买', NULL, 270000.00, '已完成'),
    (2, 5, '2025-01-25', '租赁', '2025-4-25', 10800.00, '进行中'), 
    (1, 2, '2025-02-20', '租赁', '2025-8-20', 27000.00, '进行中'), 
    (5, 16, '2025-02-28', '租赁', '2025-3-28', 3000.00, '进行中')
ON DUPLICATE KEY UPDATE
    order_date = VALUES(order_date),
    order_type = VALUES(order_type),
    end_date = VALUES(end_date),
    total_price = VALUES(total_price),
    status = VALUES(status);

CREATE TABLE IF NOT EXISTS model_price_history (
    history_id INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
    model_id INT NOT NULL,                                     -- 关联的车型
    record_date DATE NOT NULL,                                 -- 价格记录日期
    sale_price DECIMAL(10,2) NOT NULL,                         -- 购车价格
    rental_price DECIMAL(10,2) NOT NULL,                       -- 租赁价格
    FOREIGN KEY (model_id) REFERENCES models(model_id) ON DELETE CASCADE,
    UNIQUE KEY unique_model_date (model_id, record_date)       -- 确保同一车型同一天只能有一条记录
);

INSERT INTO model_price_history (model_id, record_date, sale_price, rental_price)
VALUES 
    -- 特斯拉 Model 3
    (1, '2024-01-01', 275000.00, 4400.00),
    (1, '2024-02-01', 278000.00, 4450.00),
    (1, '2024-03-01', 280000.00, 4500.00),
    (1, '2024-04-01', 282000.00, 4550.00),
    (1, '2024-05-01', 285000.00, 4600.00),

    -- 比亚迪 汉 EV
    (2, '2024-01-01', 245000.00, 3700.00),
    (2, '2024-02-01', 248000.00, 3750.00),
    (2, '2024-03-01', 250000.00, 3800.00),
    (2, '2024-04-01', 253000.00, 3850.00),
    (2, '2024-05-01', 255000.00, 3900.00),

    -- 蔚来 ET7
    (3, '2024-01-01', 440000.00, 4900.00),
    (3, '2024-02-01', 445000.00, 4950.00),
    (3, '2024-03-01', 450000.00, 5000.00),
    (3, '2024-04-01', 455000.00, 5050.00),
    (3, '2024-05-01', 460000.00, 5100.00),

    -- 小鹏 P7
    (4, '2024-01-01', 260000.00, 3900.00),
    (4, '2024-02-01', 265000.00, 3950.00),
    (4, '2024-03-01', 270000.00, 4000.00),
    (4, '2024-04-01', 275000.00, 4050.00),
    (4, '2024-05-01', 280000.00, 4100.00),

    -- 理想 L9
    (5, '2024-01-01', 450000.00, 5900.00),
    (5, '2024-02-01', 455000.00, 5950.00),
    (5, '2024-03-01', 460000.00, 6000.00),
    (5, '2024-04-01', 465000.00, 6050.00),
    (5, '2024-05-01', 470000.00, 6100.00),

    -- 极氪 001
    (6, '2024-01-01', 315000.00, 4200.00),
    (6, '2024-02-01', 318000.00, 4250.00),
    (6, '2024-03-01', 320000.00, 4300.00),
    (6, '2024-04-01', 323000.00, 4350.00),
    (6, '2024-05-01', 325000.00, 4400.00),

    -- 哪吒 S
    (7, '2024-01-01', 205000.00, 3400.00),
    (7, '2024-02-01', 208000.00, 3450.00),
    (7, '2024-03-01', 210000.00, 3500.00),
    (7, '2024-04-01', 213000.00, 3550.00),
    (7, '2024-05-01', 215000.00, 3600.00),

    -- 广汽埃安 AION Y
    (8, '2024-01-01', 155000.00, 2900.00),
    (8, '2024-02-01', 158000.00, 2950.00),
    (8, '2024-03-01', 160000.00, 3000.00),
    (8, '2024-04-01', 163000.00, 3050.00),
    (8, '2024-05-01', 165000.00, 3100.00),

    -- 宝马 iX3
    (9, '2024-01-01', 395000.00, 5400.00),
    (9, '2024-02-01', 398000.00, 5450.00),
    (9, '2024-03-01', 400000.00, 5500.00),
    (9, '2024-04-01', 403000.00, 5550.00),
    (9, '2024-05-01', 405000.00, 5600.00),

    -- 奔驰 EQE
    (10, '2024-01-01', 475000.00, 5900.00),
    (10, '2024-02-01', 478000.00, 5950.00),
    (10, '2024-03-01', 480000.00, 6000.00),
    (10, '2024-04-01', 483000.00, 6050.00),
    (10, '2024-05-01', 485000.00, 6100.00)

ON DUPLICATE KEY UPDATE 
    sale_price = VALUES(sale_price), 
    rental_price = VALUES(rental_price);
