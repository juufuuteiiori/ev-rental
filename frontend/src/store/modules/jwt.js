// jwt.js

const state = {
  token: localStorage.getItem("jwt_token") || null, // 初始化时从 localStorage 获取 token
};

const mutations = {
  setToken(state, token) {
    state.token = token;
    localStorage.setItem("jwt_token", token);
  },
  removeToken(state) {
    state.token = null;
    localStorage.removeItem("jwt_token");
  },
};

const actions = {
  login({ commit }, token) {
    commit("setToken", token);
  },
  logout({ commit }) {
    commit("removeToken");
  },
};

const getters = {
  getToken: (state) => state.token,
};

export default {
  namespaced: true, // 让模块支持命名空间
  state,
  mutations,
  actions,
  getters,
};
