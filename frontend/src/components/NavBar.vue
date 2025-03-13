<template>
  <div>
    <header :class="['navbar', isTransparent ? 'transparent' : 'solid']">
      <div class="nav-container">
        <!-- Logo -->
        <div class="logo">EV Rental</div>

        <!-- 导航菜单 -->
        <nav class="nav-links">
          <router-link to="/" class="nav-item">首页</router-link>
          <router-link to="/cars" class="nav-item">租售车辆</router-link>
          <router-link to="/orderlist" class="nav-item">订单列表</router-link>
        </nav>

        <!-- 用户交互按钮 -->
        <div class="user-menu">
          <button v-show="!isLoggedIn" @click="goToLogin" class="login-btn">
            登录 / 注册
          </button>
          <el-dropdown v-show="isLoggedIn" @command="handleCommand">
            <el-button class="menu-btn">
              <el-avatar :src="userInfo.avatar || defaultAvatar" />
            </el-button>

            <el-dropdown-menu slot="dropdown">
              <el-dropdown-item command="openUserProfile"
                >个人中心</el-dropdown-item
              >
              <el-dropdown-item command="editUserProfile"
                >修改资料</el-dropdown-item
              >
              <el-dropdown-item command="logout">退出登录</el-dropdown-item>
            </el-dropdown-menu>
          </el-dropdown>
        </div>
      </div>
    </header>
    <user-profile-dialog ref="userProfileDialog" />
    <user-edit-dialog ref="userEditDialog" />
  </div>
</template>

<script>
import UserProfileDialog from "@/components/UserProfileDialog.vue";
import userEditDialog from "@/components/UserEditDialog.vue";

export default {
  name: "NavBar",
  components: {
    UserProfileDialog,
    userEditDialog,
  },
  data() {
    return {
      isTransparent: true,
      userInfo: this.$store.state.user.userInfo,
      defaultAvatar:
        "https://cube.elemecdn.com/0/88/03b0d39583f48206768a7534e55bcpng.png",
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
    handleCommand(command) {
      if (command === "openUserProfile") {
        this.$refs.userProfileDialog.openDialog();
      } else if (command === "editUserProfile") {
        this.$refs.userEditDialog.openDialog(this.$store.state.user.userInfo);
      } else if (command === "logout") {
        this.logout();
      }
    },
    logout() {
      this.$store.dispatch("jwt/logout");
      this.$store.dispatch("user/logout");
      if (this.$route.path !== "/") {
        this.$router.push("/");
      }
    },
  },
  computed: {
    isLoggedIn() {
      return this.$store.getters["jwt/getToken"] != null;
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
  background-color: transparent;
  color: transparent;
}

.menu-btn {
  background-color: transparent;
  border: none;
  padding: 0;
  font-size: 14px;
  box-shadow: none;
}

.menu-btn:hover,
.menu-btn:active,
.menu-btn:focus {
  background-color: transparent;
}

.el-dropdown-link {
  cursor: pointer;
}
.el-icon-arrow-down {
  font-size: 12px;
}
</style>
