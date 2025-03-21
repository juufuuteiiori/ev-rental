<template>
  <div class="car-detail">
    <el-card class="car-info">
      <h2 class="car-title">{{ car.brand_name }} {{ car.model_name }}</h2>

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
      car: {}, // 车辆数据
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
        const { data } = response.data;
        this.car = data;
      } catch (error) {
        console.error("获取车辆信息失败", error);
        this.car = null;
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
  },
  computed: {
    isAdmin() {
      return this.$store.state.user.userInfo.role === "管理员";
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

.action-buttons {
  display: flex;
  justify-content: center;
  gap: 15px;
  margin-top: 10vh;
}
</style>
