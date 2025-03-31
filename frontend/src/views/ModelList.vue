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
            <p>销售价格：¥ {{ car.salePrice }}</p>
            <p>租赁价格: ¥ {{ car.rentalPrice }} / 天</p>
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
              :key="brand.id"
              :label="brand.brand_name"
              :value="brand.brand_name"
            />
          </el-select>
        </el-col>

        <el-col :span="4">
          <el-input
            v-model="filters.keyword"
            placeholder="请输入车型关键词"
            clearable
            class="custom-input"
          >
          </el-input>
        </el-col>

        <el-col :span="4">
          <el-select
            v-model="filters.salePrice"
            placeholder="选择销售价格区间"
            clearable
            class="custom-select"
          >
            <el-option label="20万以下" value="0-200000"></el-option>
            <el-option label="20-30万" value="200000-300000"></el-option>
            <el-option label="30万以上" value="300000-10000000"></el-option>
          </el-select>
        </el-col>

        <el-col :span="4">
          <el-select
            v-model="filters.rentalPrice"
            placeholder="选择租赁价格区间"
            clearable
            class="custom-select"
          >
            <el-option label="100以下" value="0-100"></el-option>
            <el-option label="100-150" value="100-150"></el-option>
            <el-option label="150以上" value="150-10000000"></el-option>
          </el-select>
        </el-col>

        <el-col :span="2.5">
          <el-button @click="dialogVisible = true">更多筛选条件</el-button>
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
          label="租赁价格（元/天）"
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

    <el-dialog :visible.sync="dialogVisible" width="50%" class="custom-dialog">
      <el-form label-width="180px" class="custom-form">
        <!-- 加速度 -->
        <el-form-item label="百公里加速" class="form-item">
          <el-select
            v-model="filters.acceleration"
            placeholder="选择加速度范围"
            clearable
            class="custom-select"
          >
            <el-option label="7秒以上" value="7-100"></el-option>
            <el-option label="4-7秒" value="4-7"></el-option>
            <el-option label="4秒以内" value="0-4"></el-option>
          </el-select>
        </el-form-item>

        <!-- 充电峰值功率 -->
        <el-form-item label="充电峰值功率" class="form-item">
          <el-select
            v-model="filters.peakPower"
            placeholder="选择充电功率"
            clearable
            class="custom-select"
          >
            <el-option label="50kW以下" value="0-50"></el-option>
            <el-option label="50-150kW" value="50-150"></el-option>
            <el-option label="150kW以上" value="150-1000"></el-option>
          </el-select>
        </el-form-item>

        <!-- 续航 -->
        <el-form-item label="续航" class="form-item">
          <el-select
            v-model="filters.range"
            placeholder="选择续航范围"
            clearable
            class="custom-select"
          >
            <el-option label="400公里以下" value="0-400"></el-option>
            <el-option label="400-600公里" value="400-600"></el-option>
            <el-option label="600公里以上" value="600-2000"></el-option>
          </el-select>
        </el-form-item>

        <!-- 座位数 -->
        <el-form-item label="座位数" class="form-item">
          <el-select
            v-model="filters.seatCount"
            placeholder="选择座位数"
            clearable
            class="custom-select"
          >
            <el-option label="5座及以下" value="0-5"></el-option>
            <el-option label="6-7座" value="6-7"></el-option>
            <el-option label="8座及以上" value="8-100"></el-option>
          </el-select>
        </el-form-item>

        <!-- 储物空间 -->
        <el-form-item label="储物空间" class="form-item">
          <el-select
            v-model="filters.storageSpace"
            placeholder="选择储物空间大小"
            clearable
            class="custom-select"
          >
            <el-option label="500L以下" value="0-500"></el-option>
            <el-option label="500-1000L" value="500-1000"></el-option>
            <el-option label="1000L以上" value="1000-10000"></el-option>
          </el-select>
        </el-form-item>

        <!-- 可用数量 -->
        <el-form-item label="可用数量" class="form-item">
          <el-select
            v-model="filters.availableNumber"
            placeholder="选择可用数量"
            clearable
            class="custom-select"
          >
            <el-option label="1-5辆" value="1-5"></el-option>
            <el-option label="5-10辆" value="5-10"></el-option>
            <el-option label="10辆以上" value="10-1000"></el-option>
          </el-select>
        </el-form-item>

        <!-- 动力类型 -->
        <el-form-item label="动力类型" class="form-item">
          <el-select
            v-model="filters.powerType"
            placeholder="选择动力类型"
            clearable
            class="custom-select"
          >
            <el-option label="电动" value="电动"></el-option>
            <el-option label="混动" value="混动"></el-option>
          </el-select>
        </el-form-item>

        <!-- 收藏筛选 -->
        <el-form-item label="收藏筛选" class="form-item">
          <el-select
            v-model="filters.onlyStar"
            placeholder="请选择"
            clearable
            class="custom-select"
          >
            <el-option label="显示全部" :value="0"></el-option>
            <el-option label="仅显示已收藏" :value="1"></el-option>
          </el-select>
        </el-form-item>
      </el-form>

      <span slot="footer" class="dialog-footer">
        <el-button type="primary" @click="dialogVisible = false"
          >确定</el-button
        >
        <el-button @click="dialogVisible = false">取消</el-button>
      </span>
    </el-dialog>
  </div>
</template>

<script>
import AddModelDialog from "@/components/AddModel.vue";
import DeleteModelDialog from "@/components/DeleteModel.vue";
import { api } from "@/api";

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
        keyword: "",
        salePrice: "",
        rentalPrice: "",
        onlyStar: "",
        acceleration: "",
        peakPower: "",
        range: "",
        seatCount: "",
        storageSpace: "",
        availableNumber: "",
        powerType: "",
      },
      dialogVisible: false,

      // 分页显示
      currentPage: 1,
      pageSize: 5,

      cars: [],
      brands: [],
      recommendedCars: [],
    };
  },

  computed: {
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
            !this.filters.keyword || car.model.includes(this.filters.keyword)
        )
        .filter(
          (car) =>
            !this.filters.salePrice ||
            this.isValueInRange(car.salePrice, this.filters.salePrice)
        )
        .filter(
          (car) =>
            !this.filters.rentalPrice ||
            this.isValueInRange(car.rentalPrice, this.filters.rentalPrice)
        )
        .filter(
          (car) =>
            !this.filters.acceleration ||
            this.isValueInRange(car.acceleration, this.filters.acceleration)
        )
        .filter(
          (car) =>
            !this.filters.peakPower ||
            this.isValueInRange(car.peak_power, this.filters.peakPower)
        )
        .filter(
          (car) =>
            !this.filters.range ||
            this.isValueInRange(car.range, this.filters.range)
        )
        .filter(
          (car) =>
            !this.filters.seatCount ||
            this.isValueInRange(car.seat_count, this.filters.seatCount)
        )
        .filter(
          (car) =>
            !this.filters.storageSpace ||
            this.isValueInRange(car.storage_space, this.filters.storageSpace)
        )
        .filter(
          (car) =>
            !this.filters.powerType || car.power_type == this.filters.powerType
        )
        .filter(
          (car) =>
            !this.filters.availableNumber ||
            this.isValueInRange(
              car.available_number,
              this.filters.availableNumber
            )
        )
        .filter((car) => !this.filters.onlyStar || car.is_star === 1);
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
    addModel() {
      this.$refs.AddModelDialog.openDialog();
    },

    deleteModel() {
      this.$refs.DeleteModelDialog.openDialog(this.cars);
    },

    // 区间筛选
    isValueInRange(price, range) {
      const [min, max] = range.split("-").map(Number);
      return price >= min && price <= max;
    },

    // 跳转车辆详情页
    goToDetail(id) {
      this.$router.push(`/model/${id}`);
    },
    goToDetail2(row) {
      this.$router.push(`/model/${row.model_id}`);
    },

    // 改变分页
    changePage(page) {
      this.currentPage = page;
      window.scrollTo({ top: window.innerHeight * 1, behavior: "smooth" });
    },

    getImageUrl(path) {
      return `http://localhost:8081/image?path=${encodeURIComponent(path)}`;
    },

    async fetchCars() {
      try {
        const response = await api.getModelList();
        this.cars = response.data.vehicles;
      } catch (error) {
        this.$message.error(error.response.data.msg);
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

    async fetchRecommendedCars() {
      try {
        const response = await api.getRecommendedCars();
        this.recommendedCars = response.data.recommendedVehicles;
      } catch (error) {
        this.$message.error(error.response.data.msg);
      }
    },
  },

  watch: {
    filteredCars() {
      this.currentPage = 1;
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

.custom-dialog .el-form {
  margin-top: 20px;
  padding: 10px;
  background-color: #f9f9f9;
  border-radius: 10px;
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

.dialog-footer {
  display: flex;
  justify-content: flex-end;
  margin-top: 20px;
}

.el-button {
  margin-left: 10px;
  font-size: 14px;
  border-radius: 4px;
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
