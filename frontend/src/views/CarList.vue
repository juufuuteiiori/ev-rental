<template>
  <div class="car-list">
    <!--  推荐展示区域 -->
    <el-carousel height="100vh">
      <el-carousel-item v-for="car in recommendedCars" :key="car.model_id">
        <div class="car-card" @click="goToDetail(car.model_id)">
          <img :src="getImageUrl(car.image)" alt="推荐车辆" class="car-image" />
          <div class="car-info">
            <h3>
              {{ car.brand_name || "未知品牌" }}
              {{ car.model_name || "未知车型" }}
            </h3>

            <p>续航：{{ car.range }} km</p>
            <p>销售价格：¥ {{ car.salePrice }} 起</p>
            <p>租赁价格: ¥ {{ car.rentalPrice }} / 月起</p>
          </div>
        </div>
      </el-carousel-item>
    </el-carousel>

    <!--  搜索 & 筛选区域 -->
    <el-card class="filter-bar">
      <el-row :gutter="30">
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

        <el-col v-if="isAdmin" :span="2">
          <el-button @click="addModel">添加车型</el-button>
        </el-col>

        <el-col v-if="isAdmin" :span="2">
          <el-button @click="deleteModel">删除车型</el-button>
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
          prop="model"
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
          label="销售价格（元）"
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
    <add-model-dialog ref="AddModelDialog" />
    <delete-model-dialog ref="DeleteModelDialog" />
  </div>
</template>

<script>
import { mapState, mapActions } from "vuex";
import AddModelDialog from "@/components/AddModel.vue";
import DeleteModelDialog from "@/components/DeleteModel.vue";

export default {
  components: {
    AddModelDialog,
    DeleteModelDialog,
  },
  data() {
    return {
      // 筛选项
      filters: {
        brand: "",
        salePrice: "",
        rentalPrice: "",
        keyword: "",
      },

      // 销售价格选项
      salePriceOptions: [
        { label: "20万以下", value: "0-200000" },
        { label: "20-30万", value: "200000-300000" },
        { label: "30万以上", value: "300000-10000000" },
      ],

      // 租赁价格选项
      rentalPriceOptions: [
        { label: "3000以下", value: "0-3000" },
        { label: "3000-4000", value: "3000-4000" },
        { label: "4000以上", value: "4000-10000000" },
      ],

      // 分页显示
      currentPage: 1,
      pageSize: 5,
    };
  },

  computed: {
    // 绑定 Vuex state 到组件
    ...mapState("cars", ["cars", "brands", "recommendedCars"]),

    isAdmin() {
      return this.$store.state.user.userInfo.role === "管理员";
    },

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
            !this.filters.keyword || car.model.includes(this.filters.keyword)
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
    // 组件加载时获取数据
    this.fetchCars();
    this.fetchBrands();
    this.fetchRecommendedCars();

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
    // 绑定 Vuex action 到组件
    ...mapActions("cars", ["fetchCars", "fetchBrands", "fetchRecommendedCars"]),

    addModel() {
      this.$refs.AddModelDialog.openDialog();
    },

    deleteModel() {
      this.$refs.DeleteModelDialog.openDialog();
    },

    // 价格区间筛选逻辑
    isPriceInRange(price, range) {
      const [min, max] = range.split("-").map(Number);
      return price >= min && price <= max;
    },

    // 跳转车辆详情页
    goToDetail(id) {
      this.$router.push(`/car/${id}`);
    },
    goToDetail2(row) {
      this.$router.push(`/car/${row.model_id}`);
    },

    // 改变分页
    changePage(page) {
      this.currentPage = page;
    },

    getImageUrl(path) {
      return `http://localhost:8081/image?path=${encodeURIComponent(path)}`;
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
