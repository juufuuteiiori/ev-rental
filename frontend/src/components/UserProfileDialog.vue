<template>
  <el-dialog
    title="个人中心"
    :visible.sync="dialogVisible"
    width="50%"
    :modal="true"
  >
    <div class="user-info">
      <!-- 用户头像 -->
      <el-avatar
        :size="150"
        :src="userInfo.avatar || defaultAvatar"
        class="user-avatar"
      />

      <el-descriptions border column="1" class="descriptions-container">
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
    </div>

    <span slot="footer">
      <el-button type="primary" @click="dialogVisible = false">关闭</el-button>
    </span>
  </el-dialog>
</template>

<script>
export default {
  data() {
    return {
      dialogVisible: false,
      userInfo: this.$store.state.user.userInfo,
      defaultAvatar:
        "https://cube.elemecdn.com/0/88/03b0d39583f48206768a7534e55bcpng.png",
    };
  },
  methods: {
    openDialog() {
      this.dialogVisible = true;
    },
  },
};
</script>

<style scoped>
.user-info {
  text-align: center;
}

.user-avatar {
  margin-bottom: 20px;
}

.descriptions-container {
  margin: 0 10%;
}
</style>
