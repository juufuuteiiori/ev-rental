import Vue from "vue";
import VueRouter from "vue-router";
import store from "@/store";
import HomeView from "@/views/HomeView.vue";
import ErrorPage from "@/views/ErrorPage.vue";
import LogIn from "@/views/LogIn.vue";
import ModelList from "@/views/ModelList.vue";
import ModelDetail from "@/views/ModelDetail.vue";
import OrderPage from "@/views/OrderPage.vue";
import OrderList from "@/views/OrderList.vue";
import DiscussionPage from "@/views/DiscussionPage.vue";
import UserPage from "@/views/UserPage.vue";
import VehicleList from "@/views/VehicleList.vue";

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
    path: "/Models",
    name: "ModelList",
    component: ModelList,
  },
  {
    path: "/Model/:id",
    name: "ModelDetail",
    component: ModelDetail,
  },
  {
    path: "/order/:id",
    name: "OrderPage",
    component: OrderPage,
  },
  {
    path: "/orderlist",
    name: "OrderList",
    component: OrderList,
  },
  {
    path: "/discussion",
    name: "DiscussionPage",
    component: DiscussionPage,
  },
  {
    path: "/vehicles",
    name: "VehicleList",
    component: VehicleList,
  },
  {
    path: "/users",
    name: "users",
    component: UserPage,
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

router.beforeEach((to, from, next) => {
  const isLoggedIn = store.getters["jwt/getToken"]; // 从 Vuex 获取登录状态

  if (isLoggedIn && to.path === "/login") {
    next("/404"); // 跳转到 404
  } else {
    next(); // 允许访问其他路由
  }
});

export default router; // 导出实例
