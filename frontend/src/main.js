import Vue from "vue";
import App from "./App.vue";
import router from "./router"; // 引入路由配置

import ElementUI from "element-ui";
import "element-ui/lib/theme-chalk/index.css";

import VueAwesomeSwiper from "vue-awesome-swiper";
import "swiper/css/swiper.css";

Vue.use(ElementUI);
Vue.use(VueAwesomeSwiper);

Vue.config.productionTip = false;

new Vue({
  router, // 挂载路由
  render: (h) => h(App),
}).$mount("#app");
