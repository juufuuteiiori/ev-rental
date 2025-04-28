# ev-rental

## 前端

进入 `frontend` 文件夹，使用 `npm run build` 生成静态文件。

使用 `node server.js` 启动前端。

## 后端

进入 `backend` 文件夹，新建 `build` 文件夹并使用 CMake 完成代码编译。

进入 `build` 文件夹，使用 `./server` 启动后端。

在第一次运行时，使用 `./server -i` 初始化数据库。数据库文件位于 `backend/database/database.sql`。

## 数据库

新建空的 `ev_rental_database` 数据库。

相关配置请修改 `backend/database/config.json`

## NLP 服务器

进入 `nlpserver` 文件夹，使用 `python3 app.py` 启动 NLP 服务器
