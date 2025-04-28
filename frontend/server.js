const express = require("express");
const path = require("path");
const history = require("connect-history-api-fallback");

const app = express();
const port = 8080;

// 支持 history 模式的前端路由
app.use(history());

// 托管静态文件
app.use(express.static(path.join(__dirname, "dist")));

// 启动服务
app.listen(port, () => {
  console.log(`Server is running on http://localhost:${port}`);
});
