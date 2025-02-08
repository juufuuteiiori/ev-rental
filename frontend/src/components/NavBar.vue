<template>
  <header :class="{ scrolled: isScrolled }" class="navbar">
    <div class="nav-container">
      <!-- Logo -->
      <div class="logo">EV Rental</div>

      <!-- 导航菜单 -->
      <nav class="nav-links">
        <router-link to="/" class="nav-item">首页</router-link>
        <router-link to="/rental" class="nav-item">租赁车辆</router-link>
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
      isScrolled: false,
      isLoggedIn: false, // 模拟登录状态，实际从 vuex 中得到
    };
  },
  mounted() {
    window.addEventListener("scroll", this.handleScroll);
  },
  beforeUnmount() {
    window.removeEventListener("scroll", this.handleScroll);
  },
  methods: {
    handleScroll() {
      this.isScrolled = window.scrollY > 50;
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
/* 初始状态（透明背景 + 白色字体） */
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
  background: transparent;
  color: white;
}

/* 滚动后的状态（白色背景 + 黑色字体） */
.navbar.scrolled {
  background: white;
  box-shadow: 0 2px 10px rgba(0, 0, 0, 0.1);
  color: black;
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

.navbar.scrolled .nav-item {
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

.navbar.scrolled .login-btn {
  border-color: black;
  color: black;
}

.login-btn:hover {
  background: rgba(255, 255, 255, 0.2);
}

/* 下拉菜单 */
.user-menu {
  display: flex;
  align-items: center;
}

.user-name {
  cursor: pointer;
  font-size: 16px;
  margin-right: 10px;
}
</style>
