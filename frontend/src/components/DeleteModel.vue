<template>
  <!-- 删除车型的弹窗 -->
  <el-dialog
    title="删除车型"
    :visible.sync="dialogVisible"
    width="50%"
    :modal="true"
    class="model-delete-dialog"
  >
    <!-- 车型选择框 -->
    <el-select v-model="selectedCarId" placeholder="请选择要删除的车型">
      <el-option
        v-for="car in cars"
        :key="car.model_id"
        :label="car.brand + ' ' + car.model"
        :value="car.model_id"
      ></el-option>
    </el-select>

    <!-- 删除提示信息 -->
    <el-form label-width="20%">
      <p>⚠️ 确定要删除该车型吗？此操作无法撤销！</p>
    </el-form>

    <!-- 底部按钮 -->
    <span slot="footer">
      <el-button type="danger" @click="delModel">删除车型信息</el-button>
      <el-button @click="dialogVisible = false"> 关闭</el-button>
    </span>
  </el-dialog>
</template>

<script>
import { api } from "@/api";

export default {
  data() {
    return {
      dialogVisible: false,
      cars: [],
      selectedCarId: null,
    };
  },
  methods: {
    openDialog(cars) {
      this.dialogVisible = true;
      this.$set(this, "cars", cars);
    },

    async delModel() {
      try {
        const response = await api.delModel(this.selectedCarId);
        this.$message.success(response.data.msg);
      } catch (error) {
        this.$message.error(error.response.data.msg);
      }
    },
  },
};
</script>

<style scoped>
/* 让 el-dialog 内部内容更有层次 */
.model-delete-dialog {
  text-align: center;
}

/* 让 el-select 选择框更宽，匹配表单 */
.model-delete-dialog .el-select {
  width: 60%;
  margin-bottom: 50px;
}

/* 让弹窗内文字居中，增加间距 */
.model-delete-dialog p {
  font-size: 16px;
  font-weight: bold;
  color: #ff4d4f;
  margin-bottom: 20px;
}

/* 优化按钮布局 */
.model-delete-dialog .el-button {
  min-width: 120px;
}

.model-delete-dialog .el-button:first-child {
  margin-right: 10px;
}
</style>
