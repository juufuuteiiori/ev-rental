<template>
  <el-dialog title="修改个人资料" :visible.sync="dialogVisible" width="50%">
    <el-form :model="form" :rules="rules" ref="userForm" label-width="100px">
      <el-form-item label="用户名" prop="user_name">
        <el-input
          v-model="form.user_name"
          placeholder="请输入用户名"
        ></el-input>
      </el-form-item>
      <el-form-item label="手机号" prop="user_phone">
        <el-input
          v-model="form.user_phone"
          placeholder="请输入手机号"
        ></el-input>
      </el-form-item>

      <!-- 新增修改密码和确认密码输入框 -->
      <el-form-item label="修改密码" prop="new_password">
        <el-input
          v-model="form.new_password"
          placeholder="请输入新密码"
          show-password
        ></el-input>
      </el-form-item>
      <el-form-item label="确认密码" prop="confirm_password">
        <el-input
          v-model="form.confirm_password"
          placeholder="请再次输入新密码"
          show-password
        ></el-input>
      </el-form-item>
    </el-form>

    <span slot="footer">
      <el-button @click="dialogVisible = false">取消</el-button>
      <el-button type="primary" @click="submitForm">保存</el-button>
    </span>
  </el-dialog>
</template>

<script>
import { api } from "@/api";
import { mapActions } from "vuex";

export default {
  data() {
    return {
      dialogVisible: false,
      form: {
        user_name: "",
        user_phone: "",
        new_password: "",
        confirm_password: "",
      },
      rules: {
        user_name: [
          { required: true, message: "请输入用户名", trigger: "blur" },
        ],
        new_password: [{ validator: this.validatePasswords, trigger: "blur" }],
        confirm_password: [
          { validator: this.validatePasswords, trigger: "blur" },
        ],
      },
    };
  },
  methods: {
    ...mapActions("user", ["fetchUser"]),
    openDialog(userInfo) {
      this.form = { ...userInfo }; // 预填充数据
      this.dialogVisible = true;
    },
    validatePasswords(rule, value, callback) {
      if (
        (this.form.new_password && !this.form.confirm_password) ||
        (!this.form.new_password && this.form.confirm_password)
      ) {
        callback(new Error("修改密码和确认密码必须同时填写"));
      } else if (this.form.new_password !== this.form.confirm_password) {
        callback(new Error("两次输入的密码不一致"));
      } else {
        callback();
      }
    },
    async submitForm() {
      this.dialogVisible = false;
      const valid = await this.$refs.userForm.validate();
      if (!valid) {
        console.log("表单验证失败");
        return;
      }

      const requestData = {
        user_name: this.form.user_name,
        user_phone: this.form.user_phone,
        user_password: this.form.new_password,
      };

      try {
        const response = await api.updateUser(requestData);

        const { code, msg } = response.data;
        if (code === 1) {
          this.$message.success("修改成功");
          this.fetchUser(this.$store.state.user.userInfo.user_id);
        } else {
          this.$message.error(msg);
        }
      } catch (error) {
        if (error.response && error.response.status === 409) {
          this.$message.error(error.response.data.msg);
        } else {
          console.error("API 请求错误:", error);
          this.$message.error(`网络异常，请稍后重试`);
        }
      }
    },
  },
};
</script>
