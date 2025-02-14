<template>
  <div class="car-list">
    <!--  推荐展示区域 -->
    <el-carousel height="100vh">
      <el-carousel-item v-for="car in recommendedCars" :key="car.id">
        <div class="car-card" @click="goToDetail(car.id)">
          <img :src="car.imgSrc" alt="推荐车辆" class="car-image" />
          <div class="car-info">
            <h3>{{ car.name }}</h3>
            <p>续航：{{ car.range }}</p>
            <p>销售价格：{{ car.salePrice }}</p>
            <p>租赁价格: {{ car.rentalPrice }}</p>
          </div>
        </div>
      </el-carousel-item>
    </el-carousel>

    <!--  搜索 & 筛选区域 -->
    <el-card class="filter-bar">
      <el-row :gutter="20">
        <el-col :span="5">
          <el-select
            v-model="filters.brand"
            placeholder="选择品牌"
            clearable
            class="custom-select"
          >
            <el-option
              v-for="brand in brandOptions"
              :key="brand"
              :label="brand"
              :value="brand"
            />
          </el-select>
        </el-col>

        <el-col :span="5">
          <el-select
            v-model="filters.salePrice"
            placeholder="选择销售价格区间"
            clearable
            class="custom-select"
          >
            <el-option
              v-for="price in salePriceOptions"
              :key="price.value"
              :label="price.label"
              :value="price.value"
            />
          </el-select>
        </el-col>

        <el-col :span="5">
          <el-select
            v-model="filters.rentalPrice"
            placeholder="选择租赁价格区间"
            clearable
            class="custom-select"
          >
            <el-option
              v-for="price in rentalPriceOptions"
              :key="price.value"
              :label="price.label"
              :value="price.value"
            />
          </el-select>
        </el-col>

        <el-col :span="5">
          <el-input
            v-model="filters.keyword"
            placeholder="请输入车型或关键词"
            clearable
            class="custom-input"
            @keyup.enter="fetchCars"
          >
            <template #append>
              <el-button icon="el-icon-search" @click="fetchCars" />
            </template>
          </el-input>
        </el-col>
      </el-row>
    </el-card>

    <!-- 车辆列表（表格） -->
    <div class="table-container">
      <el-table
        :data="paginatedCars"
        stripe
        border
        style="width: 100%; min-height: 288px"
        @row-click="goToDetail2"
      >
        <el-table-column
          prop="brand"
          label="品牌"
          :flex-grow="1"
          align="center"
          header-align="center"
        />

        <el-table-column
          prop="name"
          label="车型"
          :flex-grow="2"
          align="center"
          header-align="center"
        />

        <el-table-column
          prop="range"
          label="续航（km）"
          :flex-grow="1"
          align="center"
          header-align="center"
        />

        <el-table-column
          prop="salePrice"
          label="销售价格（万元）"
          :flex-grow="1.5"
          align="center"
          header-align="center"
        />

        <el-table-column
          prop="rentalPrice"
          label="租赁价格（元/月）"
          :flex-grow="1.5"
          align="center"
          header-align="center"
        />
      </el-table>
    </div>

    <!-- 分页 -->
    <div class="paginated-container">
      <el-pagination
        background
        layout="prev, pager, next"
        :total="filteredCars.length"
        :page-size="pageSize"
        :current-page="currentPage"
        @current-change="changePage"
      />
    </div>
  </div>
</template>

<script>
export default {
  data() {
    return {
      recommendedCars: [
        {
          id: 1,
          name: "特斯拉 Model 3",
          range: "567 km",
          salePrice: "¥ 23.99 万起",
          rentalPrice: "¥ 4000 每月起",
          imgSrc: require("@/assets/CarList/byd_sl.jpg"),
        },
        {
          id: 2,
          name: "比亚迪 汉 EV",
          range: "605 km",
          salePrice: "¥ 21.98 万起",
          rentalPrice: "¥ 3500 每月起",
          imgSrc: require("@/assets/CarList/byd_sl.jpg"),
        },
        {
          id: 3,
          brand: "蔚来",
          name: "ES6",
          range: "562 km",
          salePrice: "¥ 34.99 万起",
          rentalPrice: "¥ 4500 每月起",
          imgSrc: require("@/assets/CarList/byd_sl.jpg"),
        },
      ],
      // 筛选项
      filters: {
        brand: "", // 选择的品牌
        salePrice: "", // 销售价格区间
        rentalPrice: "", // 租赁价格区间
        keyword: "", // 搜索关键字
      },
      // 品牌选项
      brandOptions: ["特斯拉", "比亚迪", "蔚来", "小鹏"],
      // 销售价格选项
      salePriceOptions: [
        { label: "20万以下", value: "0-20" },
        { label: "20-30万", value: "20-30" },
        { label: "30万以上", value: "30-10000000" },
      ],
      // 租赁价格选项
      rentalPriceOptions: [
        { label: "3000以下", value: "0-3000" },
        { label: "3000-4000", value: "3000-4000" },
        { label: "4000以上", value: "4000-10000000" },
      ],
      // 车辆数据
      cars: [
        {
          id: 1,
          brand: "特斯拉",
          name: "Model 3",
          range: 600,
          rentalPrice: 4000,
          salePrice: 23.99,
        },
        {
          id: 2,
          brand: "比亚迪",
          name: "汉 EV",
          range: 550,
          rentalPrice: 3500,
          salePrice: 21.98,
        },
        {
          id: 3,
          brand: "蔚来",
          name: "ES6",
          range: 580,
          rentalPrice: 4500,
          salePrice: 34.99,
        },
        {
          id: 4,
          brand: "小鹏",
          name: "P7",
          range: 670,
          rentalPrice: 3700,
          salePrice: 24.98,
        },
        {
          id: 5,
          brand: "小鹏",
          name: "P7",
          range: 670,
          rentalPrice: 3700,
          salePrice: 24.98,
        },
        {
          id: 6,
          brand: "小鹏",
          name: "P7",
          range: 670,
          rentalPrice: 3700,
          salePrice: 24.98,
        },
      ],
      // 当前页码
      currentPage: 1,
      pageSize: 5, // 每页显示数量
    };
  },

  computed: {
    // 计算筛选后的车辆列表
    filteredCars() {
      return this.cars
        .filter(
          (car) => !this.filters.brand || car.brand === this.filters.brand
        )
        .filter(
          (car) =>
            !this.filters.salePrice ||
            this.isPriceInRange(car.salePrice, this.filters.salePrice)
        )
        .filter(
          (car) =>
            !this.filters.rentalPrice ||
            this.isPriceInRange(car.rentalPrice, this.filters.rentalPrice)
        )
        .filter(
          (car) =>
            !this.filters.keyword || car.name.includes(this.filters.keyword)
        );
    },
    paginatedCars() {
      return this.filteredCars.slice(
        (this.currentPage - 1) * this.pageSize,
        this.currentPage * this.pageSize
      );
    },
  },

  created() {
    // 从 sessionStorage 读取筛选条件
    const savedFilters = sessionStorage.getItem("carListFilters");
    if (savedFilters) {
      this.filters = JSON.parse(savedFilters);
    }
  },

  beforeUnmount() {
    // 退出页面时，保存筛选条件
    sessionStorage.setItem("carListFilters", JSON.stringify(this.filters));
  },

  methods: {
    // 价格区间筛选逻辑
    isPriceInRange(price, range) {
      const [min, max] = range.split("-").map(Number);
      return price >= min && price <= max;
    },
    // 获取数据（未来可对接API）
    fetchCars() {
      console.log("筛选条件:", this.filters);
      this.currentPage = 1; // 搜索时回到第一页
    },
    // 跳转车辆详情页
    goToDetail(id) {
      this.$router.push(`/car/${id}`);
    },
    goToDetail2(row) {
      this.$router.push(`/car/${row.id}`);
    },
    // 改变分页
    changePage(page) {
      this.currentPage = page;
    },
  },
};
</script>

<style scoped>
.car-list {
  margin: 0;
  display: flex;
  flex-direction: column;
}

.car-card {
  position: relative;
  cursor: pointer;
  text-align: left;
  height: 100vh; /* 确保 .car-card 高度与轮播高度一致 */
  overflow: hidden; /* 避免超出部分可见 */
}

.car-image {
  width: 100%;
  height: 100%; /* 确保填充整个容器 */
  object-fit: cover; /* 让图片填充整个容器，超出部分裁剪 */
  object-position: center center; /* 图片中心对齐，裁剪上下部分 */
}

.car-info {
  padding: 40px;
  font-size: 20px;
  color: white;
  position: absolute;
  bottom: 0;
  width: 100%;
}

.filter-bar {
  background-color: #f8f9fa; /* 浅灰色背景 */
  padding: 15px;
  padding-left: 3vw;
  border-radius: 12px; /* 圆角 */
  box-shadow: 0 4px 10px rgba(0, 0, 0, 0.05); /* 轻微阴影 */
  transition: all 0.3s ease-in-out;
}

.custom-select,
.custom-input {
  width: 100%;
  border-radius: 8px; /* 让输入框更圆润 */
}

/* 选中时的高亮效果 */
.custom-select :deep(.el-input__inner),
.custom-input :deep(.el-input__inner) {
  transition: all 0.3s ease-in-out;
  border-radius: 8px;
}

.custom-select :deep(.el-input__inner):hover,
.custom-input :deep(.el-input__inner):hover {
  border-color: #409eff; /* Element-UI 主题色 */
}

.custom-select :deep(.el-input__inner):focus,
.custom-input :deep(.el-input__inner):focus {
  border-color: #007bff; /* 选中时颜色更深 */
  box-shadow: 0 0 8px rgba(64, 158, 255, 0.3);
}

.table-container {
  width: 95%;
  margin: 2.5% auto; /* 水平居中 */
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
</style>
