<template>
  <div class="user-list">
    <el-table
      :data="paginatedUsers"
      border
      stripe
      style="margin: 5%; width: 90%; min-height: 288px"
    >
      <el-table-column
        prop="user_id"
        label="用户编号"
        :flex-grow="1"
        align="center"
        header-align="center"
      ></el-table-column>
      <el-table-column
        prop="user_photo"
        label="用户头像"
        :flex-grow="2"
        align="center"
        header-align="center"
      >
        <template v-slot="scope">
          <el-avatar
            :size="60"
            :src="getImageUrl(scope.row.user_photo)"
            class="user-avatar"
          />
        </template>
      </el-table-column>

      <el-table-column
        prop="user_name"
        label="用户名"
        :flex-grow="2"
        align="center"
        header-align="center"
      ></el-table-column>
      <el-table-column
        prop="user_phone"
        label="联系方式"
        :flex-grow="2"
        align="center"
        header-align="center"
      ></el-table-column>
    </el-table>

    <div class="paginated-container">
      <el-pagination
        background
        layout="prev, pager, next"
        :total="this.users.length"
        :page-size="pageSize"
        :current-page="currentPage"
        @current-change="changePage"
      />
    </div>
  </div>
</template>

<script>
import { api } from "@/api";
export default {
  data() {
    return {
      users: [], // 分页显示
      currentPage: 1,
      pageSize: 5,
    };
  },

  methods: {
    async fetchAllUser() {
      try {
        const response = await api.getAllUser();
        this.$set(this, "users", response.data.users);
      } catch (error) {
        this.$message.error(error.response.data.msg);
      }
    },

    getImageUrl(path) {
      if (path === undefined || path === null) {
        path = ""; // 赋值空字符串
      }
      return `http://localhost:8081/image?path=${encodeURIComponent(path)}`;
    },

    // 改变分页
    changePage(page) {
      this.currentPage = page;
    },
  },

  created() {
    this.fetchAllUser();
  },

  computed: {
    paginatedUsers() {
      return this.users.slice(
        (this.currentPage - 1) * this.pageSize,
        this.currentPage * this.pageSize
      );
    },
  },
};
</script>

<style scoped>
.user-list {
  display: flex;
  flex-direction: column;
  margin: 0;
  padding-top: 5%;
  min-height: 70.1vh;
}

.paginated-container {
  display: flex;
  justify-content: center; /* 水平居中 */
  margin: 2%;
}

.paginated-container .el-pagination {
  font-size: 2vw; /* 随视口宽度变化 */
}

.paginated-container .el-pagination button {
  width: 4vw; /* 让按钮宽度随视口调整 */
  height: 1vw; /* 让按钮高度随视口调整 */
  min-width: 40px; /* 防止过小 */
  min-height: 40px;
}
</style>
