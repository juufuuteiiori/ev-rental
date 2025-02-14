import Vue from "vue";
import VueRouter from "vue-router";
import HomeView from "@/views/HomeView.vue";
import ErrorPage from "@/views/ErrorPage.vue";
import LogIn from "@/views/LogIn.vue";
import CarList from "@/views/CarList.vue";
import CarDetail from "@/views/CarDetail.vue";

Vue.use(VueRouter); // 路由管理工具

const routes = [
  {
    path: "/",
    name: "HomeView",
    component: HomeView,
  },
  {
    path: "*",
    name: "ErrorPage",
    component: ErrorPage,
  },
  {
    path: "/login",
    name: "LogIn",
    component: LogIn,
  },
  {
    path: "/cars",
    name: "CarList",
    component: CarList,
  },
  {
    path: "/car/:id",
    name: "CarDetail",
    component: CarDetail,
  },
]; // 定义路由规则

const router = new VueRouter({
  mode: "history", // 使用 history 模式，去掉 URL 中的 #
  routes,
  scrollBehavior(to, from, savedPosition) {
    if (savedPosition) {
      return savedPosition; // 返回时恢复上次滚动位置
    } else {
      return { x: 0, y: 0 }; // 进入新页面时回到顶部
    }
  },
}); // 创建实例

export default router; // 导出实例
