import Vue from "vue";
import Vuex from "vuex";
import cars from "./modules/cars";
import jwt from "./modules/jwt";
import user from "./modules/user";

Vue.use(Vuex);

export default new Vuex.Store({
  modules: {
    cars,
    jwt,
    user,
  },
});
