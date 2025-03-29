<template>
  <el-dialog
    title="添加车型"
    :visible.sync="dialogVisible"
    width="50%"
    :modal="true"
  >
    <div class="model-form">
      <el-form
        ref="modelForm"
        :model="modelData"
        :rules="rules"
        label-width="20%"
      >
        <!-- 车型图片 -->
        <el-form-item label="车型图片" prop="images">
          <el-upload
            class="upload-demo"
            action=""
            :http-request="handleUpload"
            :show-file-list="false"
            :multiple="true"
            accept="image/*"
          >
            <el-button type="primary" style="margin-left: 70%"
              >上传图片</el-button
            >
          </el-upload>

          <!-- 图片预览 -->
          <div class="image-preview">
            <div
              v-for="(url, index) in modelData.imageUrls"
              :key="index"
              class="image-item"
            >
              <img :src="getImageUrl(url)" class="preview-img" />
              <el-button type="danger" size="mini" @click="removeImage(index)"
                >删除</el-button
              >
            </div>
          </div>
        </el-form-item>

        <!-- 车型品牌 -->
        <el-form-item label="车型品牌" prop="model_brand">
          <el-input
            v-model="modelData.model_brand"
            placeholder="请输入车型品牌"
            class="short-input"
          ></el-input>
        </el-form-item>

        <!-- 车型名称 -->
        <el-form-item label="车型名称" prop="model_name">
          <el-input
            v-model="modelData.model_name"
            placeholder="请输入车型名称"
            class="short-input"
          ></el-input>
        </el-form-item>

        <!-- 续航里程 -->
        <el-form-item label="续航 (km)" prop="range">
          <el-input
            v-model.number="modelData.range"
            placeholder="请输入续航里程"
            type="number"
            class="short-input"
          >
            <template slot="append">km</template>
          </el-input>
        </el-form-item>

        <!-- 动力类型 -->
        <el-form-item label="动力类型" prop="power_type">
          <el-select
            v-model="modelData.power_type"
            placeholder="选择动力类型"
            style="margin-left: 10%"
          >
            <el-option label="电动" value="电动"></el-option>
            <el-option label="混动" value="混动"></el-option>
          </el-select>
        </el-form-item>

        <!-- 销售价格 -->
        <el-form-item label="销售价格 (元)" prop="sale_price">
          <el-input
            v-model.number="modelData.sale_price"
            placeholder="请输入销售价格"
            type="number"
            class="short-input"
          ></el-input>
        </el-form-item>

        <!-- 租赁价格 -->
        <el-form-item label="租赁价格 (元/月)" prop="rental_price">
          <el-input
            v-model.number="modelData.rental_price"
            placeholder="请输入租赁价格"
            type="number"
            class="short-input"
          ></el-input>
        </el-form-item>

        <!-- 充电峰值功率 -->
        <el-form-item label="充电峰值功率 (kW)" prop="charging_power">
          <el-input
            v-model.number="modelData.charging_power"
            placeholder="请输入充电功率"
            type="number"
            class="short-input"
          ></el-input>
        </el-form-item>

        <!-- 加速时间 -->
        <el-form-item label="百公里加速 (秒)" prop="acceleration">
          <el-input
            v-model.number="modelData.acceleration"
            placeholder="请输入加速时间"
            type="number"
            class="short-input"
          ></el-input>
        </el-form-item>

        <!-- 座位数 -->
        <el-form-item label="座位数" prop="seats">
          <el-input
            v-model.number="modelData.seats"
            placeholder="请输入座位数"
            type="number"
            class="short-input"
          ></el-input>
        </el-form-item>

        <!-- 储物空间 -->
        <el-form-item label="储物空间 (L)" prop="storage_space">
          <el-input
            v-model.number="modelData.storage_space"
            placeholder="请输入储物空间"
            type="number"
            class="short-input"
          ></el-input>
        </el-form-item>
      </el-form>
    </div>

    <span slot="footer">
      <!-- 提交按钮 -->
      <el-button type="primary" @click="submitModel">提交车型信息</el-button>
      <el-button type="danger" @click="dialogVisible = false">关闭</el-button>
    </span>
  </el-dialog>
</template>

<script>
import { api } from "@/api";

export default {
  data() {
    return {
      dialogVisible: false,
      modelData: {
        model_brand: "",
        model_name: "",
        imageUrls: [],
        range: null,
        power_type: "",
        sale_price: null,
        rental_price: null,
        charging_power: null,
        acceleration: null,
        seats: null,
        storage_space: null,
      },
      rules: {
        model_brand: [
          { required: true, message: "请输入车型品牌", trigger: "blur" },
        ],
        model_name: [
          { required: true, message: "请输入车型名称", trigger: "blur" },
        ],
        range: [{ required: true, message: "请输入续航里程", trigger: "blur" }],
        power_type: [
          { required: true, message: "请选择动力类型", trigger: "change" },
        ],
        sale_price: [
          { required: true, message: "请输入销售价格", trigger: "blur" },
        ],
        rental_price: [
          { required: true, message: "请输入租赁价格", trigger: "blur" },
        ],
        charging_power: [
          { required: true, message: "请输入充电功率", trigger: "blur" },
        ],
        acceleration: [
          { required: true, message: "请输入百公里加速时间", trigger: "blur" },
        ],
        seats: [{ required: true, message: "请输入座位数", trigger: "blur" }],
        storage_space: [
          { required: true, message: "请输入储物空间", trigger: "blur" },
        ],
      },
    };
  },
  methods: {
    openDialog() {
      this.dialogVisible = true;
    },

    getImageUrl(path) {
      return `http://localhost:8081/image?path=${encodeURIComponent(path)}`;
    },

    async handleUpload(image) {
      const formData = new FormData();
      formData.append("image", image.file);

      try {
        const response = await api.postImage(formData); // 调用上传 API
        this.modelData.imageUrls.push(response.data.file); // 存储多个图片
        this.$message.success(response.data.msg);
      } catch (error) {
        this.$message.error(error.response.data.msg);
      }
    },

    removeImage(index) {
      this.modelData.imageUrls.splice(index, 1); // 从数组中删除
    },

    async submitModel() {
      // const valid = this.$refs.modelForm.validate();

      const valid = await new Promise((resolve) => {
        this.$refs.modelForm.validate((isValid) => resolve(isValid));
      });

      if (!valid) {
        this.$message.error("请填写完整的车型信息！");
        return;
      }

      try {
        const response = await api.submitModel(this.modelData);
        this.dialogVisible = false;
        this.$message.success(response.data.msg);
      } catch (error) {
        this.$message.error(error.response.data.msg);
      }
    },
  },
  computed: {},
};
</script>

<style scoped>
.short-input {
  margin-left: 10%;
  width: 60%;
}

.image-preview {
  display: flex;
  flex-wrap: wrap;
  margin-top: 10px;
}

.image-item {
  position: relative;
  margin-right: 10px;
}

.preview-img {
  width: 100px;
  height: 80px;
  border-radius: 5px;
  object-fit: cover;
}

.image-item .el-button {
  position: absolute;
  top: 5px;
  right: 5px;
}
</style>
