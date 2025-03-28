<template>
  <div class="login-container">
    <!-- 左侧登录框 -->
    <el-card class="login-card">
      <h2 class="login-title">{{ isLogin ? "账号登录" : "账号注册" }}</h2>

      <el-form ref="loginForm" :model="form" :rules="rules" label-width="80px">
        <el-form-item :label="computedLabel" prop="username">
          <el-input
            v-model="form.username"
            :placeholder="`请输入${computedLabel}`"
            class="input-limited"
            clearable
          ></el-input>
        </el-form-item>

        <el-form-item label="密码" prop="password">
          <el-input
            v-model="form.password"
            placeholder="请输入密码"
            class="input-limited"
            show-password
          ></el-input>
        </el-form-item>

        <!-- 只有注册时才显示“确认密码” -->
        <el-form-item v-if="!isLogin" label="确认密码" prop="confirmPassword">
          <el-input
            v-model="form.confirmPassword"
            placeholder="请再次输入密码"
            class="input-limited"
            show-password
          ></el-input>
        </el-form-item>

        <el-form-item>
          <el-button type="primary" class="submit-btn" @click="handleSubmit">
            {{ isLogin ? "登录" : "注册" }}
          </el-button>
        </el-form-item>

        <!-- 提供“去注册 / 去登录”按钮 -->
        <el-form-item>
          <el-button type="text" class="swap-btn" @click="toggleAuthMode">
            {{ isLogin ? "没有账号？去注册" : "已有账号？去登录" }}
          </el-button>
        </el-form-item>
      </el-form>
    </el-card>

    <!-- 右侧文字信息 -->
    <div class="info-text">
      <h2>加入我们，开启绿色出行</h2>
      <p class="subtitle">租购电动车，从未如此简单</p>

      <div class="highlights">
        <div class="highlight-item">
          <h3>🚀 轻松租车</h3>
          <p>按月、按年租赁，随时随地用车。</p>
        </div>

        <div class="highlight-item">
          <h3>💳 快速注册</h3>
          <p>仅需用户名密码即可注册，享受优质服务。</p>
        </div>

        <div class="highlight-item">
          <h3>🔒 安全保障</h3>
          <p>多重加密保护您的个人信息。</p>
        </div>
      </div>
    </div>
  </div>
</template>

<script>
import { api } from "@/api";

export default {
  data() {
    return {
      isLogin: true, // true=登录模式，false=注册模式
      usertype: "用户",
      form: {
        username: "",
        password: "",
        confirmPassword: "", // 仅注册时使用
      },
      rules: {
        username: [
          { required: true, message: "请输入用户名", trigger: "blur" },
        ],
        password: [{ required: true, message: "请输入密码", trigger: "blur" }],
        confirmPassword: [
          { required: true, message: "请确认密码", trigger: "blur" },
          { validator: this.validatePasswordMatch, trigger: "blur" }, // 添加自定义验证
        ],
      },
    };
  },
  computed: {
    computedLabel() {
      return this.usertype === "用户" ? "用户名" : "员工号";
    },
  },
  methods: {
    async handleSubmit() {
      const valid = await new Promise((resolve) => {
        this.$refs.loginForm.validate((isValid) => resolve(isValid));
      });

      if (!valid) {
        return;
      }

      try {
        const requestData = {
          user_name: this.form.username,
          user_password: this.form.password,
        };

        const response = this.isLogin
          ? await api.loginUser(requestData)
          : await api.registerUser(requestData);

        this.$store.dispatch("jwt/login", response.data.token);
        this.$store.dispatch("user/fetchUser", response.data.user_id);
        this.$router.push("/");
        this.$message.success(response.data.msg);
      } catch (error) {
        this.$message.error(error.response.data.msg);
      }
    },

    toggleAuthMode() {
      if (this.usertype === "员工" && this.isLogin) {
        this.$message.warning("员工账号不支持注册");
        return;
      }
      this.isLogin = !this.isLogin;
    },

    validatePasswordMatch(rule, value, callback) {
      if (!this.isLogin && value !== this.form.password) {
        callback(new Error("两次密码不一致"));
      } else {
        callback();
      }
    },
  },

  watch: {
    // 监听 usertype 变化
    usertype() {
      // 1. 清空输入框
      this.form.username = "";
      this.form.password = "";
      this.form.confirmPassword = "";

      // 2. 更新验证规则
      this.rules.username[0].message = `请输入${this.computedLabel}`;

      // 3. 清除已有的验证状态
      this.$nextTick(() => {
        this.$refs.loginForm.clearValidate([
          "username",
          "password",
          "confirmPassword",
        ]);
      });

      // 4. 登录、注册按钮
      if (this.usertype === "员工") {
        this.isLogin = true;
      }
    },
  },
};
</script>

<style scoped>
.login-container {
  display: flex;
  align-items: center;
  justify-content: space-between;
  height: 100vh;
  padding: 0 10%;
}

.login-card {
  width: 30vw;
  height: 70vh;
  margin-top: 10vh;
  padding: 5vh;
  border-radius: 8px;
  box-shadow: 0 2px 10px rgba(0, 0, 0, 0.1);
}

.login-title {
  text-align: center;
  margin-bottom: 80px;
}

.user-type {
  display: flex;
  justify-content: center;
  margin-bottom: 10%;
}

.input-limited {
  width: 80%;
}

.submit-btn {
  margin-top: 5%;
  margin-bottom: 5%;
  width: 80%;
}

.swap-btn {
  margin-left: 15%;
}

.info-text {
  width: 25vw;
  margin-right: 5vw;
  text-align: left;
}

.subtitle {
  font-size: 16px;
  color: #555;
  margin-bottom: 20px;
}

.highlights {
  display: flex;
  flex-direction: column;
  gap: 15px;
}

.highlight-item h3 {
  font-size: 18px;
  color: #333;
}

.highlight-item p {
  font-size: 14px;
  color: #666;
}
</style>
