import Vue from "vue";
import VueRouter from "vue-router";
import HomeView from "@/views/HomeView.vue"; // 确保 HomeView.vue 放在 `src/views/` 目录下
import ErrorPage from "@/views/ErrorPage.vue";
import LogIn from "@/views/LogIn.vue";

Vue.use(VueRouter); // 路由管理工具

const routes = [
  {
    path: "/",
    name: "HomeView",
    component: HomeView,
  },
  {
    path: "/404",
    name: "ErrorPage",
    component: ErrorPage,
  },
  {
    path: "/login",
    name: "LogIn",
    component: LogIn,
  },
]; // 定义路由规则

const router = new VueRouter({
  mode: "history", // 使用 history 模式，去掉 URL 中的 #
  routes,
}); // 创建实例

export default router; // 导出实例
