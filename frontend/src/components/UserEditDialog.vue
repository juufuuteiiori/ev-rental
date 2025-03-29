<template>
  <el-dialog title="修改个人资料" :visible.sync="dialogVisible" width="50%">
    <el-form :model="form" :rules="rules" ref="userForm" label-width="100px">
      <el-form-item label="用户头像" prop="user_photo">
        <el-avatar
          :size="150"
          :src="getImageUrl(form.user_photo)"
          class="user-avatar"
        />

        <el-upload
          class="upload-demo"
          action=""
          :http-request="handleUpload"
          :show-file-list="false"
          :multiple="true"
          accept="image/*"
        >
          <el-button type="primary" style="margin: 10% 2%">修改图片</el-button>
        </el-upload>
      </el-form-item>

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
      const valid = await new Promise((resolve) => {
        this.$refs.userForm.validate((isValid) => resolve(isValid));
      });

      if (!valid) {
        this.$message.error("表单信息错误");
        return;
      }

      const requestData = {
        user_name: this.form.user_name,
        user_phone: this.form.user_phone,
        user_password: this.form.new_password,
        user_photo: this.form.user_photo,
      };

      try {
        const response = await api.updateUser(requestData);
        this.$store.dispatch(
          "user/fetchUser",
          this.$store.state.user.userInfo.user_id
        );
        this.$message.success(response.data.msg);
      } catch (error) {
        this.$message.error(error.response.data.msg);
      }
    },

    getImageUrl(path) {
      // 处理 path 为 undefined 或空字符串的情况
      if (path === undefined || path === null) {
        path = ""; // 赋值空字符串
      }

      return `http://localhost:8081/image?path=${encodeURIComponent(path)}`;
    },

    async handleUpload(image) {
      const formData = new FormData();
      formData.append("image", image.file);

      try {
        const response = await api.postImage(formData); // 调用上传 API
        this.$set(this.form, "user_photo", response.data.file);
        this.$message.success(response.data.msg);
      } catch (error) {
        this.$message.error(error.response.data.msg);
      }
    },
  },
};
</script>

<style scoped>
.image-preview {
  display: flex;
  flex-wrap: wrap;
  margin-top: 10px;
}

.image-item {
  position: relative;
  margin-right: 10px;
}

.preview-img {
  width: 100px;
  height: 80px;
  border-radius: 5px;
  object-fit: cover;
}
</style>
