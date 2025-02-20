import { api } from "@/api";

const state = {
  cars: [],
  isCarsLoaded: false,
  brands: [],
  isBrandsLoaded: false,
};

const mutations = {
  setCars(state, cars) {
    state.cars = cars || [];
    state.isCarsLoaded = true;
  },
  setBrands(state, brands) {
    state.brands = brands || [];
    state.isBrandsLoaded = true;
  },
};

const actions = {
  async fetchCars({ state, commit }) {
    if (state.isCarsLoaded) return;

    try {
      const response = await api.getVehicleList();
      const { code, msg, data } = response.data; // 解构提取

      if (code === 0) {
        console.error("获取车辆数据失败:", msg);
        commit("setCars", []);
      } else if (Array.isArray(data)) {
        commit("setCars", data);
      } else {
        console.warn("返回数据格式异常:", response.data);
        commit("setCars", []);
      }
    } catch (error) {
      console.error("获取车辆数据失败:", error);
      commit("setCars", []);
    }
  },

  async fetchBrands({ state, commit }) {
    if (state.isBrandsLoaded) return;

    try {
      const response = await api.getBrandList();
      const { code, msg, data } = response.data; // 解构提取

      if (code === 0) {
        console.error("获取品牌数据失败:", msg);
        commit("setBrands", []);
      } else if (Array.isArray(data)) {
        commit("setBrands", data);
      } else {
        console.warn("返回数据格式异常:", response.data);
        commit("setBrands", []);
      }
    } catch (error) {
      console.error("获取品牌数据失败:", error);
      commit("setBrands", []);
    }
  },
};

export default {
  namespaced: true, // 让模块支持命名空间
  state,
  mutations,
  actions,
};
