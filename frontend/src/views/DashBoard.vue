<template>
  <el-card class="dashboard-card">
    <h2>用户信息</h2>
    <el-descriptions title="个人信息" border>
      <el-descriptions-item label="用户 ID">{{
        userInfo.user_id
      }}</el-descriptions-item>
      <el-descriptions-item label="用户名">{{
        userInfo.user_name
      }}</el-descriptions-item>
      <el-descriptions-item label="手机号">{{
        userInfo.user_phone
      }}</el-descriptions-item>
    </el-descriptions>
  </el-card>
</template>

<script>
import { mapGetters } from "vuex";

export default {
  computed: {
    ...mapGetters("user", ["getUserInfo"]),
    userInfo() {
      return this.getUserInfo || {}; // 确保 userInfo 存在
    },
  },
};
</script>

<style scoped>
.dashboard-card {
  max-width: 600px;
  margin: 20px auto;
}
</style>
