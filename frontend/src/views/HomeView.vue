<template>
    <div class="home">
      <el-row class="header">
        <el-col :span="24">
          <el-header>
            <h1>电动车租赁系统</h1>
          </el-header>
        </el-col>
      </el-row>
  
      <el-row class="content">
        <el-col :span="24" class="main-content">
          <el-card>
            <h2>欢迎使用电动车租赁系统！</h2>
            <el-button type="primary">立即租赁</el-button>
          </el-card>
        </el-col>
      </el-row>
  
      <el-footer>
        <p>© 2025 电动车租赁系统</p>
      </el-footer>
    </div>
  </template>
  
  <script>
  export default {
    name: 'HomeView',
    data() {
      return {};
    },
    methods: {},
  };
  </script>
  
  <style scoped>
  .home {
    background-color: #f4f4f4;
  }
  
  .header {
    background-color: #409EFF;
    text-align: center;
    padding: 20px 0;
  }
  
  .main-content {
    padding: 30px;
    text-align: center;
  }
  
  .el-footer {
    text-align: center;
    padding: 20px;
    background-color: #fff;
    margin-top: 20px;
  }
  </style>
  