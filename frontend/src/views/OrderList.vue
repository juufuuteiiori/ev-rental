<template>
  <div class="order-list">
    <!-- 用户未登录时提示 -->
    <div v-if="!isLoggedIn" class="login-message">
      <p>请先登录以查看订单信息。</p>
      <el-button type="primary" @click="goToLogin">去登录</el-button>
    </div>

    <!-- 用户已登录，显示订单表格 -->
    <div v-else>
      <el-table
        :data="paginatedOrders"
        border
        stripe
        style="width: 100%; min-height: 288px"
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
          prop="model_name"
          label="车型"
          :flex-grow="1"
          align="center"
          header-align="center"
        ></el-table-column>
        <el-table-column
          prop="license_plate"
          label="车牌号"
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
          :total="orders.length"
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
        <el-descriptions-item label="车型"
          >{{ selectedOrder.brand_name }}
          {{ selectedOrder.model_name }}</el-descriptions-item
        >
        <el-descriptions-item label="车牌号">{{
          selectedOrder.license_plate
        }}</el-descriptions-item>
        <el-descriptions-item label="订单类型">{{
          selectedOrder.order_type
        }}</el-descriptions-item>
        <el-descriptions-item
          v-if="selectedOrder.order_type === '租赁'"
          label="结束时间"
        >
          {{ selectedOrder.end_date }}
        </el-descriptions-item>
        <el-descriptions-item label="总金额">{{
          selectedOrder.total_price
        }}</el-descriptions-item>
      </el-descriptions>
      <span slot="footer">
        <el-button @click="dialogVisible = false">关闭</el-button>
      </span>
    </el-dialog>
  </div>
</template>

<script>
import { api } from "@/api";
import { mapState } from "vuex";

export default {
  data() {
    return {
      isLoggedIn: false, // 是否已登录
      orders: [], // 订单数据
      dialogVisible: false, // 弹窗状态
      selectedOrder: {}, // 选中的订单详情

      // 分页显示
      currentPage: 1,
      pageSize: 5,
    };
  },

  computed: {
    ...mapState("user", ["userInfo"]),
    paginatedOrders() {
      return this.orders.slice(
        (this.currentPage - 1) * this.pageSize,
        this.currentPage * this.pageSize
      );
    },
  },

  created() {
    this.checkLogin();
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
        const response = await api.getOrders(this.userInfo.user_id);
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
      } catch (error) {
        console.error("获取订单失败", error);
        this.$message.error("无法加载订单数据");
      }
    },

    // 点击某一行，弹出订单详情
    openOrderDetail(row) {
      this.dialogVisible = true;
      this.fetchOrderById(row.order_id);
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
  padding: 8%;
  height: 46.4vh;
}
.login-message {
  text-align: center;
  padding: 20px;
  margin: auto auto;
}

.el-table {
  width: 100%; /* 让表格填充容器 */
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
</style>
