USE ev_rental_database;

-- 用户表
CREATE TABLE IF NOT EXISTS users (
    user_id INT NOT NULL AUTO_INCREMENT PRIMARY KEY,           -- 用户的唯一标识
    user_name VARCHAR(50) NOT NULL UNIQUE,                     -- 用户名
    user_phone VARCHAR(15) UNIQUE,                             -- 用户的手机号
    user_password VARCHAR(255) NOT NULL,                       -- 用户的登录密码
    user_photo TEXT,                                           -- 图片路径
    role ENUM('用户', '管理员') NOT NULL DEFAULT '用户'         -- 权限管理 
);

INSERT INTO users (user_name, user_phone, user_password, role, user_photo) VALUES
    ('alice', '13800000001', '$2a$12$Hl5x.7DMV672OSyhatWmb.46Ei.zfzXjNu1JZ60LStBw7yIB0xING', '用户', '6fd7eab7c7_Fy17u79XoAAMbl7.jpg'),
    ('bob', '13800000002', '$2a$12$Df5rFcexYWgfqLO3Yk2laOi16lHYHW3AVnGT/ECJU6AWGX8.Bip6C', '用户', NULL),
    ('charlie', '13800000003', '$2a$12$/G3SDM0skF0nbDWUEmYjW.VDcAuM5Eld700nxz/tLjCQZVXSjJI8u', '用户', NULL),
    ('david', '13800000004', '$2a$12$qeTR9nkR.2GRBQH0z9ubUOimoAag.StU25/wNmtqeRK1bzyCxYqti', '用户', NULL),
    ('eve', '13800000005', '$2a$12$Qg63lFuLfmaMtCBSHDWZJ.euD07RUOPggxE1Oh0.TqvsEtbo4.tOq', '用户', NULL),
    ('iori', '13800000006', '$2a$12$Qg63lFuLfmaMtCBSHDWZJ.euD07RUOPggxE1Oh0.TqvsEtbo4.tOq', '管理员', '4ffd8163b9_FzE4zeWWAAU-WMI.jpg'); -- 默认密码为 "password"

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
    recommend BOOLEAN NOT NULL DEFAULT FALSE                   -- 是否为推荐车辆
);

INSERT INTO models (brand_name, model_name, power_type, max_range, leasing_price, purchase_price, peak_power, acceleration, seat_count, storage_space, image_paths, recommend)
VALUES
    ('特斯拉', 'Model 3', '电动', 634, 132.00, 250000.00, 170.00, 6.1, 5, 682, 'Tesla_Model3_1.png,Tesla_Model3_2.png,Tesla_Model3_3.jpg', TRUE), -- 后轮驱动版
    ('比亚迪', '汉 EV 尊贵型', '电动', 506, 100.00, 220000.00, 200.00, 7.9, 5, 500, 'BYD_HEV_1.jpg,BYD_HEV_2.jpg,BYD_HEV_3.jpg,BYD_HEV_4.jpg', TRUE), -- 2025款 506KM 尊贵型
    ('蔚来', 'ET7 行政版', '电动', 1050, 154.00, 417000.00, 280.00, 3.8, 5, 520, 'WL_ET7_1.jpg,WL_ET7_2.jpg,WL_ET7_3.jpg,WL_ET7_4.jpg', FALSE),  -- ET7 行政版
    ('小鹏', 'P7i 702 Pro', '电动', 702, 110.00, 233000.00, 220.00, 3.9, 5, 450, 'XP_P7i_1.jpg,XP_P7i_2.jpg,XP_P7i_3.jpg,XP_P7i_4.jpg', FALSE), -- 702 Pro
    ('理想', 'L9 Pro', '混动', 1412, 177.00, 424000.00, 180.00, 5.3, 6, 700, 'LX_L9_1.jpg,LX_L9_2.jpg,LX_L9_3.png', TRUE), -- L9 Pro
    ('极氪', '001 WE 后驱', '电动', 620, 104.00, 264000.00, 240.00, 6.3, 5, 500, 'JK_001_1.webp,JK_001_2.webp,JK_001_3.webp', FALSE), -- WE 95kWh电池 后驱
    ('哪吒', 'S 520 Lite', '电动', 520, 77.00, 119500.00, 210.00, 7.4, 5, 400, 'NZ_S_1.webp,NZ_S_2.webp,NZ_S_3.webp,NZ_S_4.webp', FALSE), -- 520 Lite
    ('广汽埃安', 'AION Y Younger', '电动', 430, 27.00, 74500.00, 150.00, 6.9, 5, 380, 'GQAA_AIONY_1.png,GQAA_AIONY_2.png,GQAA_AIONY_3.png', FALSE), -- AION Y Younger
    ('宝马', 'iX3 领先型', '电动', 540, 164.00, 335000.00, 200.00, 6.8, 5, 500, 'BW_iX3_1.jpg,BW_iX3_2.jpg,BW_iX3_3.jpg', TRUE), -- 领先型
    ('奔驰', 'EQE 500 4MATIC', '电动', 681, 180.00, 366000.00, 260.00, 4.9, 5, 540, 'BZ_EQE_1.jpg,BZ_EQE_2.jpg,BZ_EQE_3.jpg,BZ_EQE_4.jpg', TRUE); -- 500 4MATIC 先锋版

-- 电动车表
CREATE TABLE IF NOT EXISTS vehicles (
    vehicle_id INT PRIMARY KEY AUTO_INCREMENT,                 -- 电动车唯一标识
    model_id INT NOT NULL,                                     -- 关联的车型 ID
    license_plate VARCHAR(20) NOT NULL UNIQUE,                 -- 车牌号
    purchase_date DATE DEFAULT (CURRENT_DATE),                 -- 购置日期
    status ENUM('可用', '已租赁', '已售出', '不可用') NOT NULL,  -- 电动车状态
    FOREIGN KEY (model_id) REFERENCES models(model_id) ON DELETE CASCADE
);

INSERT INTO vehicles (model_id, license_plate, purchase_date, status)
VALUES
    -- 特斯拉 Model 3
    (1, '川A300001', '2024-01-10', '可用'), -- 1
    (1, '川A300002', '2024-02-15', '已租赁'), -- 2
    (1, '川A300003', '2024-03-20', '可用'), -- 3
    (1, '川A300004', '2024-05-05', '可用'), -- 4
    (1, '川A300005', '2024-06-18', '已租赁'), -- 5

    -- 比亚迪 汉 EV
    (2, '川A300006', '2024-01-25', '可用'), -- 6
    (2, '川A300007', '2024-03-05', '已售出'), -- 7
    (2, '川A300008', '2024-04-12', '可用'), -- 8
    (2, '川A300009', '2024-06-08', '已售出'), -- 9
    (2, '川A300010', '2024-07-15', '已售出'), -- 10

    -- 蔚来 ET7
    (3, '川A300011', '2024-02-20', '已租赁'), -- 11
    (3, '川A300012', '2024-04-25', '可用'), -- 12
    (3, '川A300013', '2024-06-10', '不可用'),
    (3, '川A300014', '2024-08-15', '已售出'), -- 14

    -- 小鹏 P7
    (4, '川A300015', '2024-01-12', '已售出'), -- 15
    (4, '川A300016', '2024-03-18', '已租赁'), -- 16
    (4, '川A300017', '2024-05-22', '可用'),
    (4, '川A300018', '2024-07-30', '可用'),

    -- 理想 L9
    (5, '川A300019', '2024-01-30', '不可用'),
    (5, '川A300020', '2024-03-10', '可用'),
    (5, '川A300021', '2024-05-15', '可用'),
    (5, '川A300022', '2024-07-20', '可用'),

    -- 极氪 001
    (6, '川A300023', '2024-02-18', '可用'),
    (6, '川A300024', '2024-04-22', '不可用'),
    (6, '川A300025', '2024-06-30', '可用'),
    (6, '川A300026', '2024-08-05', '可用'),

    -- 哪吒 S
    (7, '川A300027', '2024-01-15', '可用'),
    (7, '川A300028', '2024-03-20', '可用'),
    (7, '川A300029', '2024-05-25', '可用'),

    -- 广汽埃安 AION Y
    (8, '川A300030', '2024-02-10', '可用'),
    (8, '川A300031', '2024-04-18', '不可用'),
    (8, '川A300032', '2024-06-22', '可用'),

    -- 宝马 iX3
    (9, '川A300033', '2024-02-28', '可用'),
    (9, '川A300034', '2024-05-10', '可用'),
    (9, '川A300035', '2024-07-18', '可用'),
    (9, '川A300036', '2024-09-22', '可用'),

    -- 奔驰 EQE
    (10, '川A300037', '2024-03-08', '可用'),
    (10, '川A300038', '2024-06-15', '不可用'),
    (10, '川A300039', '2024-09-20', '可用'),
    (10, '川A300040', '2025-01-05', '可用');

CREATE TABLE IF NOT EXISTS orders (
    order_id INT NOT NULL AUTO_INCREMENT PRIMARY KEY,                     -- 订单唯一标识
    user_id INT NOT NULL,                                                 -- 关联用户表，标识下单用户
    vehicle_id INT NOT NULL,                                              -- 关联电动车表，标识租赁车辆
    address TEXT NOT NULL,                                               -- 收货地址
    payment_method ENUM('支付宝', '微信支付', "银行卡") NOT NULL,  -- 支付方式
    order_date DATE NOT NULL,                                             -- 订单创建日期
    order_type ENUM('购买', '租赁') NOT NULL,                              -- 订单类型
    end_date DATE DEFAULT NULL,                                           -- 租赁结束日期（仅租赁适用）
    total_price DECIMAL(10, 2) NOT NULL,                                  -- 订单总金额（单位：元）
    status ENUM('进行中', '已完成') NOT NULL DEFAULT '进行中',              -- 订单状态
    rating INT NULL CHECK (rating BETWEEN 1 AND 5),                       -- 用户评分
    review TEXT NULL,                                                     -- 用户评论

    CHECK (order_type = '购买' OR (end_date IS NOT NULL AND end_date >= order_date)), -- 确保租赁订单有日期
    FOREIGN KEY (user_id) REFERENCES users(user_id) ON DELETE CASCADE, -- 关联用户，用户删除时订单删除
    FOREIGN KEY (vehicle_id) REFERENCES vehicles(vehicle_id) ON DELETE CASCADE, -- 关联车辆，车辆删除时订单删除
    CONSTRAINT unique_user_vehicle_order UNIQUE (user_id, vehicle_id, order_date)
);

INSERT INTO orders (user_id, vehicle_id, order_date, address, payment_method, order_type, end_date, total_price, status, rating, review)
VALUES
    (1, 1, '2024-06-15', '四川省成都市四川大学江安校区', '微信支付', '租赁', '2024-12-15', 26700.00, '已完成', 5, '体验不错'),
    (5, 12, '2024-07-08', '四川省成都市四川大学望江校区', '支付宝', '租赁', '2025-03-08', 48000.00, '已完成', NULL, NULL),
    (4, 4, '2024-07-15', '四川省成都市电子科技大学清水河校区', '微信支付', '租赁', '2024-10-15', 13200.00, '已完成', 4, '体验还行'),
    (3, 3, '2024-08-01', '四川省成都市锦江区春熙路街道', '银行卡', '租赁', '2024-12-01', 17500.00, '已完成', 5, '下次还会选择这辆车'),
    (3, 7, '2024-09-10', '四川省成都市四川大学江安校区', '银行卡', '购买', NULL, 240000.00, '已完成', 5, '购买体验很好'),
    (5, 14, '2024-09-20', '四川省成都市锦江区中纱帽街8号大慈寺路', '支付宝', '购买', NULL, 444000.00, '已完成', 5, '取车很方便'),
    (4, 9, '2024-11-05', '四川省成都市龙泉驿区大面洪河中路351号', '银行卡', '购买', NULL, 234000.00, '已完成', 4, '质量不错'),
    (2, 5, '2025-01-25', '四川省成都市锦江区驿都大道静康路536号', '支付宝', '租赁', '2025-05-25', 16200.00, '进行中', NULL, NULL), 
    (1, 2, '2025-02-20', '四川省成都市龙泉驿区大面洪河中路351号', '银行卡', '租赁', '2025-08-20', 24000.00, '进行中', NULL, NULL), 
    (5, 16, '2025-02-28', '四川省成都市高新区天府三街198号', '微信支付', '租赁', '2025-05-28', 10200.00, '进行中', NULL, NULL),
    (2, 6, '2024-05-10', '四川省成都市锦江区驿都大道静康路536号', '银行卡', '租赁', '2024-09-10', 15600.00, '已完成', 5, '整体体验不错'),
    (4, 8, '2024-07-05', '四川省成都市四川大学江安校区', '支付宝', '租赁', '2024-12-05', 18750.00, '已完成', 4, '租赁手续简单，推荐'),
    (3, 10, '2024-10-01', '四川省成都市锦江区春熙路街道', '银行卡', '购买', NULL, 237000.00, '已完成', 5, '豪华感十足'),
    (1, 11, '2025-01-05', '四川省成都市天府新区正兴街道顺圣路178号', '银行卡', '租赁', '2025-07-05', 28200.00, '进行中', NULL, NULL),
    (2, 15, '2025-02-15', '四川省成都市高新区天府三街198号', '微信支付', '购买', NULL, 237000.00, '已完成', 4, '取车方便，服务好');

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
    (1, '2024-01-01', 275000.00, 147),
    (1, '2024-02-01', 277500.00, 148),
    (1, '2024-03-01', 279000.00, 149),
    (1, '2024-04-01', 281000.00, 150),
    (1, '2024-05-01', 283500.00, 150),
    (1, '2024-06-01', 278000.00, 149),
    (1, '2024-07-01', 274500.00, 147),
    (1, '2024-08-01', 272000.00, 146),
    (1, '2024-09-01', 270000.00, 144),
    (1, '2024-10-01', 267000.00, 140),
    (1, '2024-11-01', 263500.00, 139),
    (1, '2024-12-01', 260000.00, 137),
    (1, '2025-01-01', 258000.00, 135),
    (1, '2025-02-01', 255000.00, 134),
    (1, '2025-03-01', 250000.00, 132),

    -- 比亚迪 汉 EV 尊贵型
    (2, '2024-01-01', 245000.00, 124),
    (2, '2024-02-01', 247000.00, 125),
    (2, '2024-03-01', 249000.00, 127),
    (2, '2024-04-01', 251000.00, 129),
    (2, '2024-05-01', 253500.00, 130),
    (2, '2024-06-01', 249500.00, 127),
    (2, '2024-07-01', 246000.00, 125),
    (2, '2024-08-01', 243500.00, 124),
    (2, '2024-09-01', 240000.00, 120),
    (2, '2024-10-01', 237000.00, 117),
    (2, '2024-11-01', 234000.00, 114),
    (2, '2024-12-01', 231500.00, 110),
    (2, '2025-01-01', 228000.00, 107),
    (2, '2025-02-01', 225000.00, 104),
    (2, '2025-03-01', 220000.00, 100),

    -- 蔚来 ET7 行政版
    (3, '2024-01-01', 440000.00, 164),
    (3, '2024-02-01', 445500.00, 166),
    (3, '2024-03-01', 451000.00, 167),
    (3, '2024-04-01', 457000.00, 169),
    (3, '2024-05-01', 462500.00, 170),
    (3, '2024-06-01', 457500.00, 169),
    (3, '2024-07-01', 453000.00, 167),
    (3, '2024-08-01', 448500.00, 166),
    (3, '2024-09-01', 444000.00, 164),
    (3, '2024-10-01', 439500.00, 162),
    (3, '2024-11-01', 435000.00, 160),
    (3, '2024-12-01', 430500.00, 159),
    (3, '2025-01-01', 426000.00, 157),
    (3, '2025-02-01', 421500.00, 155),
    (3, '2025-03-01', 417000.00, 154),

    -- 小鹏 P7i 702 Pro
    (4, '2024-01-01', 260000.00, 130),
    (4, '2024-02-01', 263000.00, 132),
    (4, '2024-03-01', 267000.00, 134),
    (4, '2024-04-01', 271000.00, 135),
    (4, '2024-05-01', 275500.00, 137),
    (4, '2024-06-01', 270000.00, 135),
    (4, '2024-07-01', 266000.00, 134),
    (4, '2024-08-01', 261500.00, 132),
    (4, '2024-09-01', 257000.00, 130),
    (4, '2024-10-01', 253000.00, 127),
    (4, '2024-11-01', 249000.00, 124),
    (4, '2024-12-01', 245000.00, 120),
    (4, '2025-01-01', 241000.00, 117),
    (4, '2025-02-01', 237000.00, 114),
    (4, '2025-03-01', 233000.00, 110),

    -- 理想 L9 Pro
    (5, '2024-01-01', 450000.00, 197),
    (5, '2024-02-01', 454500.00, 199),
    (5, '2024-03-01', 459500.00, 200),
    (5, '2024-04-01', 464000.00, 202),
    (5, '2024-05-01', 469500.00, 204),
    (5, '2024-06-01', 464500.00, 202),
    (5, '2024-07-01', 459500.00, 200),
    (5, '2024-08-01', 455000.00, 199),
    (5, '2024-09-01', 451000.00, 197),
    (5, '2024-10-01', 446500.00, 194),
    (5, '2024-11-01', 442000.00, 190),
    (5, '2024-12-01', 437500.00, 187),
    (5, '2025-01-01', 433000.00, 184),
    (5, '2025-02-01', 428500.00, 180),
    (5, '2025-03-01', 424000.00, 177),

    -- 极氪 001 WE 后驱
    (6, '2024-01-01', 325000.00, 147),
    (6, '2024-02-01', 319000.00, 145),
    (6, '2024-03-01', 314000.00, 144),
    (6, '2024-04-01', 309000.00, 140),
    (6, '2024-05-01', 304000.00, 137),
    (6, '2024-06-01', 303000.00, 134),
    (6, '2024-07-01', 298000.00, 130),
    (6, '2024-08-01', 296000.00, 127),
    (6, '2024-09-01', 290000.00, 124),
    (6, '2024-10-01', 284000.00, 120),
    (6, '2024-11-01', 280000.00, 117),
    (6, '2024-12-01', 278000.00, 114),
    (6, '2025-01-01', 272000.00, 110),
    (6, '2025-02-01', 270000.00, 107),
    (6, '2025-03-01', 264000.00, 104),

   -- 哪吒 S 520 Lite
    (7, '2024-01-01', 154800.00, 100),
    (7, '2024-02-01', 153000.00, 99),
    (7, '2024-03-01', 149500.00, 100),
    (7, '2024-04-01', 147000.00, 95),
    (7, '2024-05-01', 144500.00, 94),
    (7, '2024-06-01', 142000.00, 95),
    (7, '2024-07-01', 140500.00, 94),
    (7, '2024-08-01', 137000.00, 89),
    (7, '2024-09-01', 134500.00, 87),
    (7, '2024-10-01', 132000.00, 85),
    (7, '2024-11-01', 129500.00, 82),
    (7, '2024-12-01', 127000.00, 80),
    (7, '2025-01-01', 124500.00, 79),
    (7, '2025-02-01', 122000.00, 77),
    (7, '2025-03-01', 119500.00, 77),

    -- 广汽埃安 AION Y Younger
    (8, '2024-01-01', 109800.00, 50),
    (8, '2024-02-01', 107000.00, 49),
    (8, '2024-03-01', 104500.00, 47),
    (8, '2024-04-01', 102000.00, 45),
    (8, '2024-05-01', 100000.00, 44),
    (8, '2024-06-01', 97000.00, 42),
    (8, '2024-07-01', 94500.00, 40),
    (8, '2024-08-01', 92000.00, 39),
    (8, '2024-09-01', 91000.00, 37),
    (8, '2024-10-01', 87000.00, 35),
    (8, '2024-11-01', 84500.00, 34),
    (8, '2024-12-01', 82000.00, 32),
    (8, '2025-01-01', 79500.00, 30),
    (8, '2025-02-01', 77000.00, 29),
    (8, '2025-03-01', 74500.00, 27),

    -- 宝马 iX3 领先型
    (9, '2024-01-01', 405000.00, 187),
    (9, '2024-02-01', 400000.00, 185),
    (9, '2024-03-01', 395000.00, 184),
    (9, '2024-04-01', 390000.00, 182),
    (9, '2024-05-01', 390000.00, 180),
    (9, '2024-06-01', 380000.00, 180),
    (9, '2024-07-01', 375000.00, 177),
    (9, '2024-08-01', 370000.00, 175),
    (9, '2024-09-01', 365000.00, 179),
    (9, '2024-10-01', 360000.00, 177),
    (9, '2024-11-01', 360000.00, 174),
    (9, '2024-12-01', 350000.00, 169),
    (9, '2025-01-01', 345000.00, 167),
    (9, '2025-02-01', 340000.00, 165),
    (9, '2025-03-01', 335000.00, 164),

    -- 奔驰 EQE 500 4MATIC
    (10, '2024-01-01', 478000.00, 204),
    (10, '2024-02-01', 470000.00, 202),
    (10, '2024-03-01', 462000.00, 200),
    (10, '2024-04-01', 454000.00, 200),
    (10, '2024-05-01', 446000.00, 197),
    (10, '2024-06-01', 438000.00, 195),
    (10, '2024-07-01', 432000.00, 194),
    (10, '2024-08-01', 422000.00, 195),
    (10, '2024-09-01', 414000.00, 192),
    (10, '2024-10-01', 406000.00, 190),
    (10, '2024-11-01', 400000.00, 187),
    (10, '2024-12-01', 390000.00, 187),
    (10, '2025-01-01', 382000.00, 184),
    (10, '2025-02-01', 374000.00, 182),
    (10, '2025-03-01', 366000.00, 180);

CREATE TABLE IF NOT EXISTS comments (
    comment_id INT PRIMARY KEY AUTO_INCREMENT,   -- 评论唯一 ID
    user_id INT NOT NULL,                        -- 发表评论的用户 ID
    parent_id INT DEFAULT NULL,                  -- 父评论 ID（若为 NULL，则为主评论）
    content TEXT NOT NULL,                       -- 评论内容
    created_at DATE DEFAULT (CURRENT_DATE),      -- 评论时间
    sentiment_score INT DEFAULT 0,               -- 存储情绪分析结果（-2 到 2）
    toxicity_score INT DEFAULT 0,                -- 存储冒犯性言论分析结果（0 或 1）
    recent_activity_date DATE DEFAULT (CURRENT_DATE),      -- 最近活跃时间
    FOREIGN KEY (user_id) REFERENCES users(user_id) ON DELETE CASCADE,
    FOREIGN KEY (parent_id) REFERENCES comments(comment_id) ON DELETE CASCADE
);

INSERT INTO comments (user_id, parent_id, content, sentiment_score, toxicity_score) 
VALUES 
    -- 插入主评论（parent_id = NULL）
    (1, NULL, '这篇文章很棒！', 2, 0),
    (2, NULL, '电动车租赁系统真有趣！', 1, 0),
    (3, NULL, '大家对续航里程有什么看法？', 0, 0),

    -- 插入子评论（回复主评论）
    (2, 1, '确实！写得很好！', 2, 0),  
    (3, 1, '有些观点我很认同！', 0, 0),  
    (1, 2, '我觉得这对行业影响很大！', 0, 0),  
    (2, 3, '电池技术发展很快，未来续航会更长！', 2, 0);

CREATE TABLE IF NOT EXISTS likes (
    like_id INT PRIMARY KEY AUTO_INCREMENT,  -- 点赞唯一 ID
    user_id INT NOT NULL,                    -- 点赞的用户 ID
    comment_id INT NOT NULL,                 -- 点赞的评论 ID
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP, -- 点赞时间
    UNIQUE KEY unique_like (user_id, comment_id),  -- 限制每个用户只能点赞一次
    FOREIGN KEY (user_id) REFERENCES users(user_id) ON DELETE CASCADE,
    FOREIGN KEY (comment_id) REFERENCES comments(comment_id) ON DELETE CASCADE
);

INSERT INTO likes (user_id, comment_id)
VALUES 
    (1, 1),
    (2, 1),
    (2, 2),
    (1, 2),
    (3, 1),
    (3, 2);

CREATE TABLE IF NOT EXISTS dislikes (
    dislike_id INT PRIMARY KEY AUTO_INCREMENT,  -- 不喜欢唯一 ID
    user_id INT NOT NULL,                    -- 不喜欢的用户 ID
    comment_id INT NOT NULL,                 -- 不喜欢的评论 ID
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP, -- 不喜欢时间
    UNIQUE KEY unique_dislike (user_id, comment_id),  -- 限制每个用户只能不喜欢一次
    FOREIGN KEY (user_id) REFERENCES users(user_id) ON DELETE CASCADE,
    FOREIGN KEY (comment_id) REFERENCES comments(comment_id) ON DELETE CASCADE
);

INSERT INTO dislikes (user_id, comment_id)
VALUES 
    (4, 1);


CREATE TABLE IF NOT EXISTS model_stars (
    model_star_id INT PRIMARY KEY AUTO_INCREMENT,
    model_id INT NOT NULL,
    user_id INT NOT NULL,
    UNIQUE KEY unique_model_user (model_id, user_id),
    FOREIGN KEY (model_id) REFERENCES models(model_id) ON DELETE CASCADE,
    FOREIGN KEY (user_id) REFERENCES users(user_id) ON DELETE CASCADE
);