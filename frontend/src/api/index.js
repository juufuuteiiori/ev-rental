import axios from "axios";

const API_BASE = process.env.VUE_APP_API_BASE_URL;

const apiClient = axios.create({
  baseURL: API_BASE, // 全局 API 前缀
  timeout: 5000,
  headers: {
    "Content-Type": "application/json",
  },
});

// 统一管理 API 请求
export const api = {
  // 获取车辆列表
  getVehicleList: () => apiClient.get("/vehicles"),

  // 获取品牌列表
  getBrandList: () => apiClient.get("/brands"),

  // 根据 ID 获取车辆信息
  getVehicleById: (vehicle_id) =>
    apiClient.get("/vehicles", { params: { vehicle_id } }),

  // 用户登录
  loginUser: (userData) => apiClient.post("/login", userData),

  // 用户注册
  registerUser: (userData) => apiClient.post("/register", userData),

  // 获取用户信息
  getUserById: (user_id) => apiClient.get("/user", { params: { user_id } }),

  // 其他 API...
};
