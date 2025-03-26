import { api } from "@/api";

const state = {
  userInfo: JSON.parse(localStorage.getItem("user_info")) || {
    user_id: 0,
    user_name: "",
    user_phone: "",
    user_photo: "",
    role: "",
  },
};

const mutations = {
  // 设置用户信息
  setUserInfo(state, userInfo) {
    state.userInfo = {
      user_id: userInfo.user_id || 0,
      user_name: userInfo.user_name || "",
      user_phone: userInfo.user_phone || "",
      user_photo: userInfo.user_photo || "",
      role: userInfo.role || "",
    };
    localStorage.setItem("user_info", JSON.stringify(state.userInfo));
  },

  // 清除用户信息
  clearUserInfo(state) {
    state.userInfo = {
      user_id: 0,
      user_name: "",
      user_phone: "",
      user_photo: "",
      role: "",
    };
    localStorage.removeItem("user_info"); // 清除存储的用户信息
  },
};

const actions = {
  // 用户登录后，存储用户信息
  setUser({ commit }, userInfo) {
    commit("setUserInfo", userInfo);
  },

  // 退出登录时清除用户信息
  logout({ commit }) {
    commit("clearUserInfo");
  },

  async fetchUser({ commit }, user_id) {
    if (!user_id) return;

    try {
      const response = await api.getUserById(user_id);
      commit("setUserInfo", response.data);
    } catch (error) {
      console.error("网络错误，获取用户信息失败：", error);
    }
  },
};

const getters = {
  // 获取用户信息
  getUserInfo: (state) => state.userInfo,
};

export default {
  namespaced: true, // 让模块支持命名空间
  state,
  mutations,
  actions,
  getters,
};
