<template>
  <div class="car-detail">
    <el-card class="car-info">
      <h2 class="car-title">
        {{ car.brand_name }} {{ car.model_name }}
        <el-button
          v-if="!isAdmin"
          :type="car.is_star === 1 ? 'primary' : 'default'"
          :icon="car.is_star === 1 ? 'el-icon-star-on' : 'el-icon-star-off'"
          style="margin-left: 10px"
          @click="toggleStar"
        >
          {{ car.is_star === 1 ? "已收藏" : "收藏" }}
        </el-button>
        <el-button
          v-if="isAdmin"
          :type="car.is_recommend === 1 ? 'primary' : 'default'"
          :icon="
            car.is_recommend === 1 ? 'el-icon-star-on' : 'el-icon-star-off'
          "
          style="margin-left: 10px"
          @click="toggleRecommend"
        >
          {{ car.is_recommend === 1 ? "已推荐" : "推荐" }}
        </el-button>
      </h2>

      <!-- 车辆图片轮播 -->
      <el-carousel class="car-carousel">
        <el-carousel-item
          v-for="(image, index) in car.image_paths"
          :key="index"
        >
          <img :src="getImageUrl(image)" class="car-image" />
        </el-carousel-item>
      </el-carousel>

      <!-- 车辆基本信息 -->
      <div>
        <div class="car-specs">
          <p><strong>续航：</strong> {{ car.max_range }} km</p>
          <p><strong>动力类型：</strong> {{ car.power_type }}</p>
          <p>
            <strong>销售价格：</strong>
            <span class="price">¥ {{ car.purchase_price }} </span>
          </p>
          <p>
            <strong>租赁价格：</strong>
            <span class="price">¥ {{ car.leasing_price }} / 月</span>
          </p>
          <p><strong>充电峰值功率：</strong> {{ car.peak_power }} kW</p>
          <p><strong>加速（百公里）：</strong> {{ car.acceleration }} s</p>
          <p><strong>座位数：</strong> {{ car.seat_count }}</p>
          <p><strong>储物空间：</strong> {{ car.storage_space }} L</p>
          <p><strong>可用数量：</strong> {{ car.available_number }}</p>
        </div>
      </div>
    </el-card>

    <!-- 数据可视化 -->
    <div class="charts-container">
      <el-card class="chart-card">
        <h3>车辆性能分析</h3>
        <div ref="radarChart" class="chart"></div>
      </el-card>

      <el-card class="chart-card">
        <h3>价格趋势分析</h3>
        <div ref="lineChart" class="chart"></div>
      </el-card>
    </div>

    <div class="comment-container">
      <p class="comment-title">用户评论</p>

      <el-card
        v-for="comment in paginatedComment"
        :key="comment.id"
        class="comment-card"
      >
        <div class="comment-header">
          <div style="width: 33%">
            <p>{{ comment.author }}</p>
          </div>

          <div style="width: 33%">
            <p>{{ comment.type }}</p>
          </div>

          <!-- 使用星星表示评分 -->
          <div>
            <i v-for="n in comment.rating" :key="n" class="el-icon-star-on"></i>
          </div>
        </div>

        <p>{{ comment.content }}</p>
      </el-card>

      <!-- 分页 -->
      <div class="paginated-container">
        <el-pagination
          background
          layout="prev, pager, next"
          :total="this.car.comments.length"
          :page-size="pageSize"
          :current-page="currentPage"
          @current-change="changePage"
        />
      </div>
    </div>

    <div class="action-buttons">
      <el-button v-if="isAdmin" type="primary" size="large" @click="updateModel"
        >修改信息</el-button
      >
      <el-button v-if="!isAdmin" type="primary" size="large" @click="handleBuy"
        >点击下单</el-button
      >
    </div>
    <update-model-dialog ref="UpdateModelDialog" />
  </div>
</template>

<script>
import * as echarts from "echarts";
import { api } from "@/api";
import UpdateModelDialog from "@/components/UpdateModel.vue";

export default {
  name: "CarDetail",
  components: {
    UpdateModelDialog,
  },
  data() {
    return {
      carId: null,
      car: {
        comments: [],
      }, // 车辆数据
      isLoading: false,

      // 分页显示
      currentPage: 1,
      pageSize: 5,
    };
  },

  methods: {
    getImageUrl(path) {
      return `http://localhost:8081/image?path=${encodeURIComponent(path)}`;
    },

    async fetchCarDetails() {
      this.carId = this.$route.params.id;
      try {
        const response = await api.getModelById(this.carId);
        this.car = response.data.vehicle;
      } catch (error) {
        this.car = null;
        this.$message.error(error.response.data.msg);
      }

      // 渲染图表
      this.renderRadarChart();

      this.renderLineChart();
    },

    renderRadarChart() {
      const radarChart = echarts.init(this.$refs.radarChart);
      const option = {
        radar: {
          indicator: [
            { name: "续航 (km)", max: 1200 },
            { name: "充电峰值功率(kW)", max: 500 },
            { name: "加速（秒百公里）", max: 10 },
            { name: "座位数", max: 10 },
            { name: "储物空间（升）", max: 700 },
          ],
        },
        series: [
          {
            type: "radar",
            data: [
              {
                value: [
                  this.car.max_range, // 续航
                  this.car.peak_power, // 充电功率
                  10 - this.car.acceleration, // 加速度
                  this.car.seat_count, // 座位数
                  this.car.storage_space, // 储物空间
                ],
                name: this.car.model_name,
              },
            ],
          },
        ],
      };
      radarChart.setOption(option);
    },

    renderLineChart() {
      const lineChart = echarts.init(this.$refs.lineChart);

      // 从历史数据中提取日期、销售价格、租赁价格
      const dates = this.car.history_prices.map((item) => item.date);
      const salePrices = this.car.history_prices.map((item) => item.salePrice);
      const rentalPrices = this.car.history_prices.map(
        (item) => item.rentalPrice
      );

      function calculateScaleFactor(range, average) {
        const ratio = range / average; // 计算 range 和 average 的比例

        if (ratio < 0.1) return 0.3; // 数据变化较小，增加系数
        if (ratio < 0.5) return 0.2; // 数据变化适中
        if (ratio < 1) return 0.1; // 数据变化较大
        return 0.05; // 数据变化很大，缩小系数
      }

      const minSale = Math.min(...salePrices);
      const maxSale = Math.max(...salePrices);
      const saleRange = maxSale - minSale;
      const saleAverage =
        salePrices.reduce((sum, value) => sum + value, 0) / salePrices.length;
      const saleFactor = calculateScaleFactor(saleRange, saleAverage);
      const minSalePrice = minSale - saleRange * saleFactor;
      const maxSalePrice = maxSale + saleRange * saleFactor;

      const minRental = Math.min(...rentalPrices);
      const maxRental = Math.max(...rentalPrices);
      const rentalRange = maxRental - minRental;
      const rentalAverage =
        rentalPrices.reduce((sum, value) => sum + value, 0) /
        rentalPrices.length;
      const rentalFactor = calculateScaleFactor(rentalRange, rentalAverage);
      const minRentalPrice = minRental - rentalRange * rentalFactor;
      const maxRentalPrice = maxRental + rentalRange * rentalFactor;

      const option = {
        title: {},
        tooltip: {
          trigger: "axis",
          axisPointer: {
            type: "cross", // 显示十字指示线
          },
        },
        legend: {
          data: ["销售价格", "租赁价格"],
        },
        xAxis: {
          type: "category",
          data: dates,
        },
        yAxis: [
          {
            type: "value",
            name: "销售价格",
            position: "left",
            min: minSalePrice,
            max: maxSalePrice,
            axisLabel: {
              formatter: "{value}",
            },
          },
          {
            type: "value",
            name: "租赁价格",
            position: "right",
            min: minRentalPrice,
            max: maxRentalPrice,
            axisLabel: {
              formatter: "{value}",
            },
          },
        ],
        series: [
          {
            name: "销售价格",
            type: "line",
            data: salePrices,
            smooth: true,
            yAxisIndex: 0, // 绑定左侧 y 轴
            color: "#3eaf7c",
          },
          {
            name: "租赁价格",
            type: "line",
            data: rentalPrices,
            smooth: true,
            yAxisIndex: 1, // 绑定右侧 y 轴
            color: "#ff9900",
          },
        ],
      };

      lineChart.setOption(option);
    },

    handleBuy() {
      this.$router.push(`/order/${this.carId}`);
    },

    updateModel() {
      console.log(this.car);
      this.$refs.UpdateModelDialog.openDialog(this.car);
    },

    async toggleStar() {
      if (this.$store.state.jwt.token == null) {
        this.$message.error("请登录账号");
        return;
      }

      if (this.isLoading) return; // 防止重复点击
      this.isLoading = true;

      try {
        if (this.car.is_star) {
          await api.delStar(this.carId);
        } else {
          await api.addStar(this.carId);
        }
        this.car.is_star = this.car.is_star === 1 ? 0 : 1; // 反转状态
        this.$message.success(this.car.is_star === 1 ? "收藏成功" : "取消收藏");
      } catch (error) {
        this.$message.error("网络错误，请稍后再试");
      } finally {
        this.isLoading = false; // 恢复按钮可点击状态
      }
    },

    async toggleRecommend() {
      if (this.$store.state.jwt.token == null) {
        this.$message.error("请登录账号");
        return;
      }

      if (this.isLoading) return; // 防止重复点击
      this.isLoading = true;

      try {
        if (this.car.is_recommend) {
          await api.delRecommend(this.carId);
        } else {
          await api.addRecommend(this.carId);
        }
        this.car.is_recommend = this.car.is_recommend === 1 ? 0 : 1; // 反转状态
        this.$message.success(
          this.car.is_recommend === 1 ? "推荐成功" : "取消推荐"
        );
      } catch (error) {
        this.$message.error("网络错误，请稍后再试");
      } finally {
        this.isLoading = false; // 恢复按钮可点击状态
      }
    },

    changePage(page) {
      this.currentPage = page;
      window.scrollTo({ top: window.innerHeight * 0.3, behavior: "smooth" });
    },
  },

  computed: {
    isAdmin() {
      return this.$store.state.user.userInfo.role === "管理员";
    },

    paginatedComment() {
      return this.car.comments.slice(
        (this.currentPage - 1) * this.pageSize,
        this.currentPage * this.pageSize
      );
    },
  },
  mounted() {
    this.fetchCarDetails();
  },
};
</script>

<style scoped>
.car-detail {
  max-width: 80%;
  margin: 5% 10%;
  padding: 20px;
}

.car-info {
  margin-bottom: 20px;
  padding: 1vw;
  border-radius: 10px;
  box-shadow: 0 2px 10px rgba(0, 0, 0, 0.1);
}

/* 标题 */
.car-title {
  font-size: 22px;
  font-weight: bold;
  margin: 15px 0;
}

.car-carousel {
  width: 40vw;
  height: 50vh;
  margin: 0 auto;
}

.el-carousel-item {
  display: flex;
  align-items: center;
  justify-content: center;
  height: 100%; /* 确保轮播项填充整个轮播盘 */
}

/* 图片轮播 */
.car-image {
  width: 100%;
  height: 100%;
  object-fit: cover;
  object-position: center center;
}

/* 车辆信息 */
.car-specs {
  display: grid;
  grid-template-columns: repeat(2, 1fr); /* 两列布局 */
  gap: 10px;
  font-size: 16px;
  color: #333;
}

.car-specs p {
  margin: 0;
  padding: 5px 0;
}

/* 价格突出显示 */
.price {
  font-size: 18px;
  font-weight: bold;
  color: #d9001b; /* 红色价格 */
}

.charts-container {
  display: flex;
  gap: 20px;
}

.chart-card {
  padding: 10px;
  width: 40vw;
  display: flex; /* 让内部元素使用 flex 布局 */
  flex-direction: column; /* 让 h3 和图表垂直排列 */
  justify-content: flex-start; /* 确保 h3 贴近顶部 */
  align-items: flex-start; /* 确保 h3 贴近左侧 */
  min-height: 50vh; /* 让 Card 高度更合理 */
}

.chart-card h3 {
  margin: 0 0 10px 0; /* 底部留白，防止 h3 贴着图表 */
  font-size: 1.2rem; /* 控制标题大小 */
}

.chart {
  width: 35vw; /* 控制宽度 */
  height: 40vh; /* 控制高度 */
}

/* 评论容器 */
.comment-container {
  padding: 20px;
  background-color: transparent; /* 背景颜色 */
  border-radius: 10px;
  box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1); /* 添加阴影 */
  margin: 0 auto; /* 居中 */
}

/* 用户评论标题 */
.comment-title {
  font-size: 20px;
  font-weight: bold;
  color: #333;
  margin-bottom: 15px;
}

/* 每一条评论卡片 */
.comment-card {
  background-color: #ffffff;
  padding: 20px;
  margin: 20px !important; /* 上边的 comment-container 有一个 margin 的修饰，覆盖了此处的 margin。采用 !important 作为临时解决方案 */
  border-radius: 8px;
  box-shadow: 0 2px 6px rgba(0, 0, 0, 0.1); /* 卡片阴影 */
  transition: transform 0.3s ease-in-out;
}

.comment-card:hover {
  transform: translateY(-5px); /* 鼠标悬停时卡片微微上移 */
}

.comment-header {
  display: flex;
  justify-content: space-between;
  font-weight: bold;
  margin-bottom: 10px;
  font-size: 20px;
}

.el-icon-star-on {
  font-size: 22px; /* 星星大小 */
  margin-right: 5px; /* 星星之间的间距 */
  color: #f39c12; /* 默认星星颜色 */
}

/* 评论内容 */
.comment-card {
  font-size: 16px;
  color: #555;
  line-height: 1.6;
  margin: 0;
}

/* 分页容器样式 */
.paginated-container {
  margin-top: 20px;
  text-align: center; /* 使分页居中 */
}

.action-buttons {
  display: flex;
  justify-content: center;
  gap: 15px;
  margin-top: 10vh;
}
</style>
