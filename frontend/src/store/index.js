import Vue from "vue";
import Vuex from "vuex";
import jwt from "./modules/jwt";
import user from "./modules/user";

Vue.use(Vuex);

export default new Vuex.Store({
  modules: {
    jwt,
    user,
  },
});
