import Vue from "vue";
import Vuex from "vuex";
import cars from "./modules/cars";

Vue.use(Vuex);

export default new Vuex.Store({
  modules: {
    cars,
  },
});
