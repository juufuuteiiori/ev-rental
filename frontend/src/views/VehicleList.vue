<template>
  <div class="vehicle-list">
    <!--  搜索 & 筛选区域 -->
    <el-card class="filter-bar">
      <el-row :gutter="16">
        <el-col :span="6">
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

        <el-col :span="6">
          <el-input
            v-model="filters.keyword"
            placeholder="请输入车型或车牌关键词"
            clearable
            class="custom-input"
          >
          </el-input>
        </el-col>

        <el-col :span="6">
          <el-select
            v-model="filters.status"
            placeholder="选择车辆状态"
            clearable
            class="custom-select"
          >
            <el-option label="可用" value="可用"></el-option>
            <el-option label="已租赁" value="已租赁"></el-option>
            <el-option label="已售出" value="已售出"></el-option>
            <el-option label="不可用" value="不可用"></el-option>
          </el-select>
        </el-col>

        <el-col :span="6">
          <el-button @click="openAddDialog">添加车辆</el-button>
        </el-col>

        <!-- <el-col :span="2">
          <el-button @click="">删除车型</el-button>
        </el-col> -->
      </el-row>
    </el-card>

    <!-- 车辆列表（表格） -->
    <el-table
      :data="paginatedVehicles"
      border
      stripe
      style="margin: 5%; width: 90%; min-height: 288px"
      @row-click="goToDetail"
    >
      <el-table-column
        prop="vehicle_id"
        label="车辆编号"
        :flex-grow="0.5"
        align="center"
        header-align="center"
      ></el-table-column>
      <el-table-column
        prop="brand"
        label="品牌"
        :flex-grow="1"
        align="center"
        header-align="center"
      ></el-table-column>
      <el-table-column
        prop="model"
        label="型号"
        :flex-grow="1"
        align="center"
        header-align="center"
      ></el-table-column>

      <el-table-column
        prop="license_plate"
        label="车牌"
        :flex-grow="1"
        align="center"
        header-align="center"
      ></el-table-column>
      <el-table-column
        prop="status"
        label="状态（点击修改）"
        :flex-grow="1"
        align="center"
        header-align="center"
      ></el-table-column>
      <el-table-column
        prop="date"
        label="添加日期"
        :flex-grow="1"
        align="center"
        header-align="center"
      ></el-table-column>
    </el-table>

    <!-- 分页 -->
    <div class="paginated-container">
      <el-pagination
        background
        layout="prev, pager, next"
        :total="this.filteredVehicles.length"
        :page-size="pageSize"
        :current-page="currentPage"
        @current-change="changePage"
      />
    </div>

    <el-dialog
      title="添加车辆"
      :visible.sync="addDialogVisible"
      width="50%"
      :modal="true"
      class="vehicle-add-dialog"
    >
      <div class="vehicle-form">
        <el-form
          ref="vehicleForm"
          :model="vehicleData"
          :rules="rules"
          label-width="20%"
        >
          <!-- 选择品牌 -->
          <el-form-item label="品牌" prop="brand">
            <el-select
              v-model="selectedBrand"
              placeholder="请选择品牌"
              @change="filterModels"
              style="margin-left: 10%"
            >
              <el-option
                v-for="brand in brands"
                :key="brand.id"
                :label="brand.brand_name"
                :value="brand.brand_name"
              />
            </el-select>
          </el-form-item>

          <!-- 选择车型 -->
          <el-form-item label="车型" prop="model_id">
            <el-select
              v-model="vehicleData.model_id"
              placeholder="请选择车型"
              style="margin-left: 10%"
            >
              <el-option
                v-for="model in filteredModels"
                :key="model.id"
                :label="model.model_name"
                :value="model.id"
              />
            </el-select>
          </el-form-item>

          <el-form-item label="车牌号" prop="license_plate">
            <el-input
              v-model="vehicleData.license_plate"
              placeholder="请输入车牌号"
              class="short-input"
            >
            </el-input>
          </el-form-item>

          <el-form-item label="车辆状态" prop="status">
            <el-select
              v-model="vehicleData.status"
              placeholder="选择车辆状态"
              style="margin-left: 10%"
            >
              <el-option label="可用" value="可用"></el-option>
              <el-option label="不可用" value="不可用"></el-option>
            </el-select>
          </el-form-item>
        </el-form>
      </div>

      <!-- 添加提示信息 -->
      <el-form>
        <p>⚠️ 此操作不可逆！ 车型与车牌号不可修改、删除！ 请谨慎操作！</p>
      </el-form>

      <span slot="footer">
        <!-- 提交按钮 -->
        <el-button type="primary" @click="submitVehicle"
          >提交车辆信息</el-button
        >
        <el-button type="danger" @click="addDialogVisible = false"
          >关闭</el-button
        >
      </span>
    </el-dialog>

    <el-dialog
      title="修改车辆"
      :visible.sync="updateDialogVisble"
      width="50%"
      :modal="true"
      class="vehicle-update-dialog"
    >
      <!-- 修改提示信息 -->
      <el-form label-width="20%">
        <p>⚠️ 请谨慎操作！</p>
      </el-form>

      <span slot="footer">
        <el-button type="danger" @click="updateVehicle">
          {{
            selectedVehicle && selectedVehicle.status === "可用"
              ? "修改为不可用"
              : "修改为可用"
          }}
        </el-button>

        <el-button type="default" @click="updateDialogVisble = false"
          >关闭</el-button
        >
      </span>
    </el-dialog>
  </div>
</template>

<script>
import { api } from "@/api";
export default {
  data() {
    return {
      filters: { brand: "", keyword: "", status: "" },
      vehicles: [],
      brands: [],

      // 分页显示
      currentPage: 1,
      pageSize: 5,

      addDialogVisible: false,
      updateDialogVisble: false,
      selectedBrand: "",
      selectedVehicle: null,

      rules: {
        model_id: [
          { required: true, message: "请选择车辆型号", trigger: "blur" },
        ],
        status: [
          { required: true, message: "请选择车辆状态", trigger: "blur" },
        ],
        license_plate: [
          { required: true, message: "请输入车牌号", trigger: "blur" },
        ],
      },

      vehicleData: {
        model_id: null,
        license_plate: "",
        status: "",
      },
    };
  },

  methods: {
    async fetchVehicles() {
      try {
        const response = await api.getVehicleList();
        this.$set(this, "vehicles", response.data.vehicles);
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

    // 改变分页
    changePage(page) {
      this.currentPage = page;
    },

    openAddDialog() {
      this.addDialogVisible = true;
    },

    openUpdateDelDialog(vehicle) {
      this.updateDialogVisble = true;
      this.selectedVehicle = vehicle;
    },

    filterModels() {
      this.vehicleData.model_id = null;
    },

    goToDetail(vehicle) {
      if (
        vehicle === null ||
        vehicle.status === "已租赁" ||
        vehicle.status === "已售出"
      )
        return;

      this.openUpdateDelDialog(vehicle);
    },

    async submitVehicle() {
      this.$refs.vehicleForm.validate();
      try {
        const response = await api.submitVehicle(this.vehicleData);
        this.addDialogVisible = false;
        this.$message.success(response.data.msg);
      } catch (error) {
        this.$message.error(error.response.data.msg);
      }
    },

    async updateVehicle() {
      try {
        const updateVehicleData = {
          vehicle_id: this.selectedVehicle.vehicle_id,
          status: this.selectedVehicle.status,
        };
        const response = await api.updateVehicle(updateVehicleData);
        this.updateDialogVisble = false;
        this.$message.success(response.data.msg);
      } catch (error) {
        this.$message.error(error.response.data.msg);
      }
    },
  },

  created() {
    this.fetchBrands();
    this.fetchVehicles();
  },

  computed: {
    filteredVehicles() {
      return this.vehicles
        .filter(
          (vehicle) =>
            !this.filters.brand || vehicle.brand === this.filters.brand
        )
        .filter(
          (vehicle) =>
            !this.filters.keyword ||
            vehicle.model.includes(this.filters.keyword) ||
            vehicle.license_plate.includes(this.filters.keyword)
        )
        .filter(
          (vehicle) =>
            !this.filters.status || vehicle.status === this.filters.status
        );
    },

    paginatedVehicles() {
      return this.filteredVehicles.slice(
        (this.currentPage - 1) * this.pageSize,
        this.currentPage * this.pageSize
      );
    },

    filteredModels() {
      return this.brands.filter((b) => b.brand_name === this.selectedBrand);
    },
  },

  watch: {
    filteredVehicles() {
      this.currentPage = 1;
    },
  },
};
</script>

<style scoped>
.vehicle-list {
  display: flex;
  flex-direction: column;
  margin: 0;
  padding-top: 6%;
  min-height: 70.1vh;
}

.filter-bar {
  background-color: #f8f9fa; /* 浅灰色背景 */
  padding: 15px;
  padding-left: 15vw;
  border-radius: 12px; /* 圆角 */
  box-shadow: 0 4px 10px rgba(0, 0, 0, 0.05); /* 轻微阴影 */
  transition: all 0.3s ease-in-out;
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

.short-input {
  margin-left: 10%;
  width: 60%;
}

.vehicle-update-dialog {
  text-align: center;
}

.vehicle-update-dialog p {
  font-size: 16px;
  font-weight: bold;
  color: #ff4d4f;
  margin-bottom: 20px;
}

.vehicle-add-dialog p {
  font-size: 16px;
  font-weight: bold;
  color: #ff4d4f;
  margin-bottom: 20px;
  text-align: center;
}

/* 优化按钮布局 */
.vehicle-update-dialog .el-button {
  min-width: 120px;
}
</style>
