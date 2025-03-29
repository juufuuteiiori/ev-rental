<template>
  <div class="footer-container">
    <!-- 顶部部分（一些链接） -->
    <el-row class="footer-top">
      <el-col :span="2">
        <el-link href="about" class="footer-link">关于我们</el-link>
      </el-col>
      <el-col :span="2">
        <el-link href="service" class="footer-link">服务条款</el-link>
      </el-col>
      <el-col :span="2">
        <el-link href="privacy" class="footer-link">隐私政策</el-link>
      </el-col>
    </el-row>

    <!-- 底部部分（版权信息） -->
    <el-row class="footer-bottom">
      <el-col :span="4"> ©2025 电动车租售系统</el-col>
      <el-col :span="4">地址: 成都市双流区川大路</el-col>
      <el-col :span="6">邮箱: 2021141461152@stu.scu.edu.cn</el-col>
      <el-col :span="6">邮编: 610207</el-col>
    </el-row>
  </div>
</template>

<script>
export default {
  name: "FooterBar", // 改为多单词名称
};
</script>

<style scoped>
.footer-container {
  background: #f5f5f5;
  padding: 5vh 3vw;
}

.footer-top {
  justify-content: flex-start;
  padding-bottom: 3vh;
  color: #666;
}

.footer-links .el-link:hover {
  color: #409eff;
}

.footer-bottom {
  color: #999;
}
</style>
