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

  // 上传品牌
  submitModel: (modelData) => apiClient.post("/model", modelData),

  // 修改品牌
  updateModel: (modelData) => apiClient.post("/model/update", modelData),

  // 删除品牌
  delModel: (model_id) => apiClient.get("/model/del", { params: { model_id } }),

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

  // 获取订单列表信息
  getOrders: (user_id) => apiClient.get("/orders", { params: { user_id } }),

  // 获取订单信息
  getOrder: (order_id) => apiClient.get("/order", { params: { order_id } }),

  // 提交订单信息
  submitOrder: (orderData) => apiClient.post("/orders", orderData),

  // 确认订单
  orderDone: (order_id) =>
    apiClient.get("/orderDone", { params: { order_id } }),

  // 上传图片
  postImage: (image) =>
    apiClient.post("/image", image, {
      headers: {
        "Content-Type": "multipart/form-data",
      },
    }),

  // 获取主评论
  getComments: () => apiClient.get("/comments"),

  // 获取子评论
  getCommentsById: (comment_id) =>
    apiClient.get("/comments/sub", { params: { comment_id } }),

  // 发表评论
  addComment: (commentData) => apiClient.post("/comments", commentData),

  // 删除评论
  delComment: (comment_id) =>
    apiClient.get("/comments/del", { params: { comment_id } }),

  // 点赞评论
  addLike: (comment_id) =>
    apiClient.get("/comments/like", { params: { comment_id } }),

  // 取消点赞评论
  delLike: (comment_id) =>
    apiClient.get("/comments/unlike", { params: { comment_id } }),

  // 不喜欢评论
  addDislike: (comment_id) =>
    apiClient.get("/comments/dislike", { params: { comment_id } }),

  // 取消不喜欢评论
  delDislike: (comment_id) =>
    apiClient.get("/comments/undislike", { params: { comment_id } }),

  // 收藏车辆
  addStar: (model_id) =>
    apiClient.get("/model/addstar", { params: { model_id } }),

  // 取消收藏车辆
  delStar: (model_id) =>
    apiClient.get("/model/delstar", { params: { model_id } }),

  // 推荐车辆
  addRecommend: (model_id) =>
    apiClient.get("/model/addrecommend", { params: { model_id } }),

  // 取消推荐车辆
  delRecommend: (model_id) =>
    apiClient.get("/model/delrecommend", { params: { model_id } }),

  // 其他 API...
};
