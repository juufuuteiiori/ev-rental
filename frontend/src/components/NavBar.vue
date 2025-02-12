<template>
  <header :class="['navbar', isTransparent ? 'transparent' : 'solid']">
    <div class="nav-container">
      <!-- Logo -->
      <div class="logo">EV Rental</div>

      <!-- 导航菜单 -->
      <nav class="nav-links">
        <router-link to="/" class="nav-item">首页</router-link>
        <router-link to="/cars" class="nav-item">租售车辆</router-link>
        <router-link to="/orders" class="nav-item">我的订单</router-link>
      </nav>

      <!-- 用户交互按钮 -->
      <div class="user-menu">
        <button v-if="!isLoggedIn" @click="goToLogin" class="login-btn">
          登录 / 注册
        </button>
        <el-dropdown v-else class="dropdown">
          <span class="user-name">用户</span>
          <template v-slot:dropdown>
            <el-dropdown-item @click="goToProfile">个人中心</el-dropdown-item>
            <el-dropdown-item @click="logout">退出登录</el-dropdown-item>
          </template>
        </el-dropdown>
      </div>
    </div>
  </header>
</template>

<script>
export default {
  name: "NavBar",
  data() {
    return {
      isTransparent: true,
      isLoggedIn: false, // 模拟登录状态，实际从 vuex 中得到
    };
  },
  watch: {
    // 监听路由变化
    $route(to) {
      this.updateNavbarStyle(to.path);
    },
  },
  mounted() {
    this.updateNavbarStyle(this.$route.path);
    window.addEventListener("scroll", this.handleScroll);
  },
  beforeUnmount() {
    window.removeEventListener("scroll", this.handleScroll);
  },
  methods: {
    updateNavbarStyle(path) {
      if ((path === "/") | (path === "/cars")) {
        this.isTransparent = true;
      } else {
        this.isTransparent = false; // 其他路由直接变成白色
      }
    },
    handleScroll() {
      if ((this.$route.path === "/") | (this.$route.path === "/cars")) {
        this.isTransparent = window.scrollY < 50; // 滚动 50px 后变色
      }
    },
    goToLogin() {
      this.$router.push("/login");
    },
    goToProfile() {
      this.$router.push("/profile");
    },
    logout() {
      this.isLoggedIn = false; // 这里应调用注销 API
      this.$router.push("/");
    },
  },
};
</script>

<style scoped>
.navbar {
  position: fixed;
  top: 0;
  left: 0;
  width: 100%;
  padding: 15px 40px;
  display: flex;
  align-items: center;
  justify-content: space-between;
  transition: all 0.3s ease-in-out;
  z-index: 1000;
}

.navbar.transparent {
  background: transparent;
  color: white;
}

.navbar.solid {
  background: white;
  color: black;
  box-shadow: 0 2px 10px rgba(0, 0, 0, 0.1);
}

/* 导航栏整体布局 */
.nav-container {
  display: flex;
  align-items: center;
  justify-content: space-between;
  width: 100%;
}

/* Logo */
.logo {
  font-size: 24px;
  font-weight: bold;
}

/* 导航链接 */
.nav-links {
  display: flex;
  gap: 30px;
}

.nav-item {
  text-decoration: none;
  font-size: 16px;
  transition: color 0.3s ease-in-out;
  color: white;
}

.nav-item:visited {
  color: white; /* 访问过的链接不变为紫色 */
}

.navbar.solid .nav-item {
  color: black;
}

.nav-item:hover {
  text-shadow: 0px 2px 5px rgba(255, 255, 255, 0.6);
}

/* 用户交互按钮 */
.login-btn {
  background: transparent;
  border: 1px solid white;
  padding: 8px 16px;
  color: white;
  cursor: pointer;
  transition: all 0.3s ease-in-out;
}

.navbar.solid .login-btn {
  border-color: black;
  color: black;
}

.login-btn:hover {
  background: rgba(255, 255, 255, 0.2);
}

/* 下拉菜单 */
.user-menu {
  margin-right: 5vw;
  display: flex;
  align-items: center;
  justify-content: center;
  max-width: 100%; /* 限制宽度 */
}

.user-name {
  cursor: pointer;
  font-size: 16px;
  margin-right: 10px;
}
</style>
