import axios from "axios";
import store from "@/store";
import router from "@/router";

const API_BASE = process.env.VUE_APP_API_BASE_URL;

const apiClient = axios.create({
  baseURL: API_BASE, // 全局 API 前缀
  timeout: 5000,
  headers: {
    "Content-Type": "application/json",
  },
});

// 使用请求拦截器来添加 Authorization 头部
apiClient.interceptors.request.use(
  (config) => {
    // 从 Vuex 获取 token
    const token = localStorage.getItem("jwt_token") || "";

    // 如果 token 存在，添加到请求头
    if (token) {
      config.headers["Authorization"] = `Bearer ${token}`;
    }

    return config; // 返回修改后的 config 对象
  },
  (error) => {
    return Promise.reject(error); // 错误处理
  }
);

apiClient.interceptors.response.use(
  (response) => response,
  (error) => {
    if (error.response && error.response.status === 401) {
      store.dispatch("jwt/logout");
      store.dispatch("user/logout");
      router.push("/login");
    }
    return Promise.reject(error);
  }
);

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

  // 修改用户信息
  updateUser: (userData) => apiClient.post("/user", userData),

  // 获取推荐车辆信息
  getRecommendedCars: () => apiClient.get("/recommended"),

  // 获取品牌车辆详细信息
  getModelById: (model_id) => apiClient.get("/model", { params: { model_id } }),

  // 其他 API...
};
