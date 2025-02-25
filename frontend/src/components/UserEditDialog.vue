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
    </el-form>

    <span slot="footer">
      <el-button @click="dialogVisible = false">取消</el-button>
      <el-button type="primary" @click="submitForm">保存</el-button>
    </span>
  </el-dialog>
</template>

<script>
export default {
  data() {
    return {
      dialogVisible: false,
      form: {
        user_name: "",
        user_phone: "",
      },
      rules: {
        user_name: [
          { required: true, message: "请输入用户名", trigger: "blur" },
        ],
        user_phone: [
          { required: true, message: "请输入手机号", trigger: "blur" },
        ],
      },
    };
  },
  methods: {
    openDialog(userInfo) {
      this.form = { ...userInfo }; // 预填充数据
      this.dialogVisible = true;
    },
    submitForm() {
      this.$refs.userForm.validate((valid) => {
        if (valid) {
          console.log("提交更新资料:", this.form);
          this.dialogVisible = false;
        }
      });
    },
  },
};
</script>
