<template>
  <div class="order-list">
    <!-- 用户未登录时提示 -->
    <div v-if="!isLoggedIn" class="login-message">
      <p>请先登录以查看订单信息。</p>
      <el-button type="primary" @click="goToLogin">去登录</el-button>
    </div>

    <!-- 用户已登录，显示订单表格 -->
    <div v-else>
      <el-card class="filter-bar">
        <el-row :gutter="20">
          <el-col :span="6">
            <el-date-picker
              v-model="filters.dateRange"
              type="daterange"
              range-separator="至"
              start-placeholder="开始日期"
              end-placeholder="结束日期"
              format="yyyy-MM-dd"
              value-format="yyyy-MM-dd"
              clearable
              class="custom-select"
            />
          </el-col>

          <el-col :span="4">
            <el-input
              v-model="filters.user_id"
              placeholder="输入用户编号"
              clearable
              class="custom-input"
            >
            </el-input>
          </el-col>

          <el-col :span="4">
            <el-select
              v-model="filters.brand"
              placeholder="选择品牌"
              clearable
              class="custom-select"
            >
              <el-option
                v-for="brand in this.brands"
                :key="brand"
                :label="brand"
                :value="brand"
              />
            </el-select>
          </el-col>

          <el-col :span="4">
            <el-select
              v-model="filters.order_type"
              placeholder="选择订单类型"
              clearable
              class="custom-select"
            >
              <el-option label="租赁" value="租赁"></el-option>
              <el-option label="购买" value="购买"></el-option>
            </el-select>
          </el-col>

          <el-col :span="5">
            <el-select
              v-model="filters.status"
              placeholder="选择订单状态"
              clearable
              class="custom-select"
            >
              <el-option label="进行中" value="进行中"></el-option>
              <el-option label="已完成" value="已完成"></el-option>
            </el-select>
          </el-col>
        </el-row>
      </el-card>

      <el-table
        :data="paginatedOrders"
        border
        stripe
        style="margin: 5%; width: 90%; min-height: 288px"
        @row-click="openOrderDetail"
      >
        <el-table-column
          prop="order_date"
          label="订单时间"
          :flex-grow="1"
          align="center"
          header-align="center"
        ></el-table-column>
        <el-table-column
          v-if="isAdmin"
          prop="user_id"
          label="用户编号"
          :flex-grow="1"
          align="center"
          header-align="center"
        ></el-table-column>
        <el-table-column
          prop="brand_name"
          label="品牌"
          :flex-grow="1"
          align="center"
          header-align="center"
        ></el-table-column>
        <el-table-column
          prop="order_type"
          label="订单类型"
          :flex-grow="1"
          align="center"
          header-align="center"
        ></el-table-column>
        <el-table-column
          prop="order_status"
          label="订单状态"
          :flex-grow="1"
          align="center"
          header-align="center"
        ></el-table-column>
      </el-table>

      <div class="paginated-container">
        <el-pagination
          background
          layout="prev, pager, next"
          :total="filteredOrders.length"
          :page-size="pageSize"
          :current-page="currentPage"
          @current-change="changePage"
        />
      </div>
    </div>

    <!-- 订单详情弹窗 -->
    <el-dialog title="订单详情" :visible.sync="dialogVisible" width="60%">
      <el-descriptions border :column="2" class="descriptions-container">
        <el-descriptions-item label="订单日期">{{
          selectedOrder.order_date
        }}</el-descriptions-item>
        <el-descriptions-item label="订单类型">{{
          selectedOrder.order_type
        }}</el-descriptions-item>
        <el-descriptions-item label="用户编号" v-if="isAdmin">{{
          selectedOrder.user_id
        }}</el-descriptions-item>
        <el-descriptions-item label="用户名" v-if="isAdmin">{{
          selectedOrder.user_name
        }}</el-descriptions-item>
        <el-descriptions-item label="用户手机" v-if="isAdmin">{{
          selectedOrder.user_phone
        }}</el-descriptions-item>
        <el-descriptions-item label="品牌"
          >{{ selectedOrder.brand_name }}
        </el-descriptions-item>
        <el-descriptions-item label="车型">{{
          selectedOrder.model_name
        }}</el-descriptions-item>
        <el-descriptions-item label="车牌号">{{
          selectedOrder.license_plate
        }}</el-descriptions-item>
        <el-descriptions-item label="总金额">{{
          selectedOrder.total_price
        }}</el-descriptions-item>
        <el-descriptions-item
          v-if="selectedOrder.order_type === '租赁'"
          label="结束时间"
        >
          {{ selectedOrder.end_date }}
        </el-descriptions-item>
      </el-descriptions>

      <div class="comment-container">
        <!-- 选择评分 -->
        <el-rate
          v-model="rating"
          show-text
          text-color="#ff9900"
          class="rating-selector"
          v-if="commentCheck"
        />

        <!-- 输入评论 -->
        <el-input
          v-model="review"
          type="textarea"
          placeholder="请输入评论..."
          rows="4"
          class="review-input"
          v-if="commentCheck"
        />
      </div>

      <span slot="footer">
        <el-button v-if="orderCheck" type="danger" @click="orderDone"
          >完成订单</el-button
        >
        <el-button
          v-if="commentCheck && !isAdmin"
          type="primary"
          @click="submitComment"
        >
          修改评论
        </el-button>
        <el-button @click="dialogVisible = false">关闭</el-button>
      </span>
    </el-dialog>
  </div>
</template>

<script>
import { api } from "@/api";

export default {
  data() {
    return {
      // 筛选项
      filters: {
        brand: "",
        order_type: "",
        status: "",
        user_id: "",
        dateRange: null,
      },

      brands: [],
      isLoggedIn: false, // 是否已登录
      orders: [], // 订单数据
      dialogVisible: false, // 弹窗状态
      selectedOrder: {}, // 选中的订单详情
      rating: null,
      review: "",

      // 分页显示
      currentPage: 1,
      pageSize: 5,
    };
  },

  computed: {
    isAdmin() {
      return this.$store.state.user.userInfo.role === "管理员";
    },

    orderCheck() {
      return (
        this.selectedOrder.order_status === "进行中" &&
        this.selectedOrder.order_type === "购买"
      );
    },

    commentCheck() {
      return this.selectedOrder.order_status === "已完成";
    },

    filteredOrders() {
      return this.orders
        .filter(
          (order) =>
            !this.filters.dateRange ||
            (new Date(this.filters.dateRange[0]) <=
              new Date(order.order_date) &&
              new Date(order.order_date) <= new Date(this.filters.dateRange[1]))
        )
        .filter(
          (order) =>
            !this.filters.user_id ||
            order.user_id === Number(this.filters.user_id)
        )
        .filter(
          (order) =>
            !this.filters.brand || order.brand_name === this.filters.brand
        )
        .filter(
          (order) =>
            !this.filters.order_type ||
            order.order_type === this.filters.order_type
        )
        .filter(
          (order) =>
            !this.filters.status || order.order_status === this.filters.status
        );
    },

    paginatedOrders() {
      return this.filteredOrders.slice(
        (this.currentPage - 1) * this.pageSize,
        this.currentPage * this.pageSize
      );
    },
  },

  created() {
    this.checkLogin();
    this.fetchBrands();
    if (this.isLoggedIn) {
      this.fetchOrders();
    }
  },

  methods: {
    // 检查用户是否登录
    checkLogin() {
      this.isLoggedIn = !!this.$store.state.jwt.token; // 通过 Vuex 判断是否有 JWT token
    },
    // 获取订单列表
    async fetchOrders() {
      try {
        const response = await api.getOrders(
          this.$store.state.user.userInfo.user_id
        );
        this.orders = response.data.orders;
      } catch (error) {
        console.error("获取订单失败", error);
        this.$message.error("无法加载订单数据");
      }
    },

    async fetchOrderById(order_id) {
      try {
        const response = await api.getOrder(order_id);
        this.selectedOrder = response.data.order;
        this.selectedOrder_id = order_id;
        this.rating = response.data.order.rating;
        this.review = response.data.order.review;
      } catch (error) {
        console.error("获取订单失败", error);
        this.$message.error("无法加载订单数据");
      }
    },

    async orderDone() {
      this.dialogVisible = false;
      try {
        await api.orderDone(this.selectedOrder_id);
        this.fetchOrders();
        this.$message.success("确认订单成功");
      } catch (error) {
        console.error("确认订单失败", error);
        this.$message.error("确认订单失败");
      }
    },

    async submitComment() {
      this.dialogVisible = false;

      if (!this.rating || !this.review) {
        this.$message.error("评分和评论不能为空");
        return;
      }

      try {
        const commentForm = {
          id: this.selectedOrder_id,
          rating: this.rating,
          review: this.review,
        };

        await api.submitComment(commentForm);
        this.$message.success("修改评价成功");
      } catch (error) {
        this.$message.error("确认评价失败");
      }
    },

    async fetchBrands() {
      try {
        const response = await api.getBrandList();
        this.brands = response.data.brands;
      } catch (error) {
        this.$message.error(error.response.data.msg);
      }
    },

    // 点击某一行，弹出订单详情
    openOrderDetail(row) {
      this.fetchOrderById(row.order_id);
      this.dialogVisible = true;
    },

    // 跳转到登录页面
    goToLogin() {
      this.$router.push("/login");
    },
    // 改变分页
    changePage(page) {
      this.currentPage = page;
    },
  },
};
</script>

<style scoped>
.order-list {
  display: flex;
  flex-direction: column;
  margin: 0;
  padding-top: 5%;
  min-height: 70.1vh;
}
.login-message {
  text-align: center;
  padding: 20px;
  margin: auto auto;
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

.descriptions-container {
  margin: 0 3%;
}

.filter-bar {
  background-color: #f8f9fa; /* 浅灰色背景 */
  padding: 15px;
  padding-left: 10vw;
  border-radius: 12px; /* 圆角 */
  box-shadow: 0 4px 10px rgba(0, 0, 0, 0.05); /* 轻微阴影 */
  transition: all 0.3s ease-in-out;
}

.comment-container {
  padding: 20px;
  background-color: #f9f9f9;
  border-radius: 10px;
  box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
  width: 95%;
  margin: 20px auto;
}

.rating-selector {
  margin-bottom: 20px;
}

.review-input {
  margin-bottom: 20px;
  resize: none; /* 防止调整输入框大小 */
}
</style>
