// jwt.js

const state = {
  token: localStorage.getItem("jwt_token") || null, // 初始化时从 localStorage 获取 token
  expiresAt: localStorage.getItem("jwt_expires_at") || null, // 记录过期时间
};

const mutations = {
  SET_TOKEN(state, { token, expiresAt }) {
    state.token = token;
    state.expiresAt = expiresAt;
    localStorage.setItem("jwt_token", token);
    localStorage.setItem("jwt_expires_at", expiresAt);
  },
  CLEAR_TOKEN(state) {
    state.token = null;
    state.expiresAt = null;
    localStorage.removeItem("jwt_token");
    localStorage.removeItem("jwt_expires_at");
  },
};

const actions = {
  login({ commit }, token) {
    const expiresAt = Date.now() + 24 * 60 * 60 * 1000; // 24 小时后过期
    commit("SET_TOKEN", { token, expiresAt });
  },
  logout({ commit }) {
    commit("CLEAR_TOKEN");
  },
  checkTokenExpiration({ commit, state }) {
    if (state.token && state.expiresAt && Date.now() > state.expiresAt) {
      commit("CLEAR_TOKEN");
      location.reload();
    }
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
