<template>
  <div class="car-detail">
    <el-card class="car-info">
      <h2 class="car-title">{{ car.brand_name }} {{ car.model_name }}</h2>

      <!-- 车辆图片轮播 -->
      <el-carousel class="car-carousel">
        <el-carousel-item v-for="(image, index) in car.images" :key="index">
          <img :src="image" class="car-image" />
        </el-carousel-item>
      </el-carousel>

      <!-- 车辆基本信息 -->
      <div>
        <div class="car-specs">
          <p><strong>续航：</strong> {{ car.max_range }} km</p>
          <p><strong>动力类型：</strong> {{ car.power_type }}</p>
          <p>
            <strong>销售价格：</strong>
            <span class="price">¥ {{ car.salePrice }} 万</span>
          </p>
          <p>
            <strong>租赁价格：</strong>
            <span class="price">¥ {{ car.rentalPrice }} / 月</span>
          </p>
          <p><strong>充电峰值功率：</strong> {{ car.charging_power }} kW</p>
          <p><strong>加速（百公里）：</strong> {{ car.acceleration }} s</p>
          <p><strong>座位数：</strong> {{ car.seats }}</p>
          <p><strong>储物空间：</strong> {{ car.trunk_space }} L</p>
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
      <el-button type="primary" size="large" @click="handleBuy"
        >点击下单</el-button
      >
    </div>
  </div>
</template>

<script>
import * as echarts from "echarts";
// import axios from "axios";

export default {
  name: "CarDetail",
  data() {
    return {
      car: {}, // 车辆数据
      carHistoryPrices: [],
    };
  },
  methods: {
    async fetchCarDetails() {
      const carId = this.$route.params.id;
      //   const response = await axios.get(`/api/cars/${carId}`);
      //   this.car = response.data;
      this.car = {
        id: carId,
        brand_name: "特斯拉",
        model_name: "Model 3",
        max_range: 600,
        rentalPrice: 4000,
        salePrice: 23.99,
        power_type: "电动",
        charging_power: 250,
        acceleration: 4.4,
        seats: 5,
        trunk_space: 682,
        available_number: 1,
        images: [
          require("@/assets/CarList/byd_sl.jpg"),
          require("@/assets/CarList/byd_sl.jpg"),
        ],
      };

      // 渲染图表
      this.renderRadarChart();

      this.carHistoryPrices = [
        { date: "2024-01-01", salePrice: 20, rentalPrice: 2 },
        { date: "2024-02-01", salePrice: 21, rentalPrice: 2.2 },
        { date: "2024-03-01", salePrice: 20.5, rentalPrice: 2.1 },
        { date: "2024-04-01", salePrice: 21.5, rentalPrice: 2.3 },
        { date: "2024-05-01", salePrice: 21.0, rentalPrice: 2.1 },
      ];
      this.renderLineChart();
    },

    renderRadarChart() {
      const radarChart = echarts.init(this.$refs.radarChart);
      const option = {
        radar: {
          indicator: [
            { name: "续航 (km)", max: 800 },
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
                  this.car.charging_power, // 充电功率
                  10 - this.car.acceleration, // 加速度
                  this.car.seats, // 座位数
                  this.car.trunk_space, // 储物空间
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
      const dates = this.carHistoryPrices.map((item) => item.date);
      const salePrices = this.carHistoryPrices.map((item) => item.salePrice);
      const rentalPrices = this.carHistoryPrices.map(
        (item) => item.rentalPrice
      );

      const option = {
        title: {},
        tooltip: {
          trigger: "axis",
        },
        legend: {
          data: ["销售价格（万元）", "租赁价格（千元）"],
        },
        xAxis: {
          type: "category",
          data: dates,
        },
        yAxis: {
          type: "value",
          axisLabel: {
            formatter: "{value} ",
          },
        },
        series: [
          {
            name: "销售价格（万元）",
            type: "line",
            data: salePrices,
            smooth: true, // 平滑折线
            color: "#3eaf7c",
          },
          {
            name: "租赁价格（千元）",
            type: "line",
            data: rentalPrices,
            smooth: true, // 平滑折线
            color: "#ff9900",
          },
        ],
      };

      lineChart.setOption(option);
    },

    handleBuy() {
      this.$router.push(`/order/${this.car.model_id}`);
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
