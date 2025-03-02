import Vue from "vue";
import App from "./App.vue";
import router from "./router"; // 引入路由配置
import store from "./store"; // 引入 Vuex store

import ElementUI from "element-ui";
import "element-ui/lib/theme-chalk/index.css";

import VueAwesomeSwiper from "vue-awesome-swiper";
import "swiper/css/swiper.css";

Vue.use(ElementUI);
Vue.use(VueAwesomeSwiper);

Vue.config.productionTip = false;

document.addEventListener("mousewheel", () => {}, { passive: true });
document.addEventListener("touchmove", () => {}, { passive: true });

new Vue({
  router, // 挂载路由
  store, // 注册 Vuex
  render: (h) => h(App),
  created() {
    this.$store.dispatch("jwt/checkTokenExpiration");
    setInterval(() => {
      this.$store.dispatch("jwt/checkTokenExpiration");
    }, 60 * 1000);
  },
}).$mount("#app");
