<template>
  <div class="not-found">
    <el-result
      icon="warning"
      title="404 Not Found"
      sub-title="抱歉，您访问的页面不存在"
    >
      <template #extra>
        <el-button type="primary" @click="goBack">返回</el-button>
      </template>
    </el-result>
  </div>
</template>

<script>
export default {
  name: "NotFound",
  methods: {
    goBack() {
      if (window.history.length > 1) {
        window.history.back(); // 返回上一页
      } else {
        this.$router.push("/"); // 如果没有上一页，跳转到首页
      }
    },
  },
};
</script>

<style scoped>
.not-found {
  display: flex;
  justify-content: center;
  align-items: center;
  height: 100vh;
  background-color: white;
}
</style>
