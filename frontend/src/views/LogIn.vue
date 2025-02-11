<template>
  <div class="login-container">
    <!-- 左侧登录框 -->
    <el-card class="login-card">
      <h2 class="login-title">账号登录</h2>

      <el-radio-group v-model="usertype" class="user-type">
        <el-radio-button label="用户"></el-radio-button>
        <el-radio-button label="员工"></el-radio-button>
      </el-radio-group>

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

        <el-form-item>
          <el-button type="primary" class="submit-btn" @click="handleSubmit">
            登录 / 注册
          </el-button>
          <p class="auto-register-hint">*未注册账号将自动完成注册</p>
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
          <p>按天、按月租赁，随时随地用车。</p>
        </div>

        <div class="highlight-item">
          <h3>💳 快速注册</h3>
          <p>仅需手机号即可注册，享受专属优惠。</p>
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
export default {
  data() {
    return {
      usertype: "用户",
      form: {
        username: "",
        password: "",
      },
      rules: {
        username: [{ required: true, message: "请输入手机", trigger: "blur" }],
        password: [{ required: true, message: "请输入密码", trigger: "blur" }],
      },
    };
  },
  computed: {
    computedLabel() {
      return this.usertype === "用户" ? "手机号" : "员工号";
    },
  },
  methods: {
    handleSubmit() {
      this.$refs.loginForm.validate((valid) => {
        if (valid) {
          console.log("登录成功:", this.form);
          // 这里可以添加 API 调用逻辑
        } else {
          console.log("表单验证失败");
        }
      });
    },
  },
  watch: {
    // 监听 usertype 变化
    usertype() {
      // 1. 清空输入框
      this.form.username = "";
      this.form.password = "";

      // 2. 更新验证规则
      this.rules.username[0].message = `请输入${this.computedLabel}`;

      // 3. 清除已有的验证状态
      this.$nextTick(() => {
        this.$refs.loginForm.clearValidate(["username", "password"]);
      });
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
  height: 60vh;
  padding: 5vh;
  border-radius: 8px;
  box-shadow: 0 2px 10px rgba(0, 0, 0, 0.1);
}

.login-title {
  text-align: center;
  margin-bottom: 20px;
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
  margin-left: 5%;
  width: 70%;
}

.auto-register-hint {
  font-size: 14px;
  color: #888;
  margin-top: 5%;
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
