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
        :src="getImageUrl(userInfo.user_photo)"
        class="user-avatar"
      />

      <el-descriptions border :column="1" class="descriptions-container">
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
      defaultAvatar:
        "https://cube.elemecdn.com/0/88/03b0d39583f48206768a7534e55bcpng.png",
    };
  },
  methods: {
    openDialog() {
      this.dialogVisible = true;
    },

    getImageUrl(path) {
      // 处理 path 为 undefined 或空字符串的情况
      if (path === undefined || path === null) {
        path = ""; // 赋值空字符串
      }

      return `http://localhost:8081/image?path=${encodeURIComponent(path)}`;
    },
  },
  computed: {
    userInfo() {
      return this.$store.state.user.userInfo;
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
