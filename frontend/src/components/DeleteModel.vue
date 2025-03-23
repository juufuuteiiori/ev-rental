<template>
  <el-dialog
    title="删除车型"
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
      </el-form>
    </div>

    <span slot="footer">
      <!-- 提交按钮 -->
      <el-button type="primary" @click="submitModel">删除车型信息</el-button>
      <el-button type="danger" @click="dialogVisible = false">关闭</el-button>
    </span>
  </el-dialog>
</template>

<script>
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
    };
  },
  methods: {
    openDialog() {
      this.dialogVisible = true;
    },

    async submitModel() {},
  },
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
