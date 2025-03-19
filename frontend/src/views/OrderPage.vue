<template>
  <div class="order-container">
    <el-card class="order-left">
      <img :src="getImageUrl(car.image_paths[0])" class="car-image" />
      <h3>{{ car.brand_name }} {{ car.model_name }}</h3>
      <p>购买： ¥ {{ car.purchase_price }}</p>
      <p>租赁： ¥ {{ car.leasing_price }} / 月</p>
    </el-card>

    <div class="order-right">
      <el-card class="order-options">
        <el-form
          ref="orderForm"
          :model="formData"
          :rules="rules"
          label-position="left"
          label-width="100px"
        >
          <h3 class="order-title">订单信息</h3>

          <el-form-item label="订单类型" prop="orderType">
            <el-radio-group v-model="formData.orderType">
              <el-radio label="购买">购买</el-radio>
              <el-radio label="租赁">租赁</el-radio>
            </el-radio-group>
          </el-form-item>

          <el-form-item
            label="租赁时长"
            prop="rentalDuration"
            v-if="formData.orderType === '租赁'"
          >
            <el-select
              v-model="formData.rentalDuration"
              placeholder="选择租赁时长"
              style="width: 300px"
            >
              <el-option label="1 个月" :value="1" />
              <el-option label="3 个月" :value="3" />
              <el-option label="6 个月" :value="6" />
              <el-option label="12 个月" :value="12" />
            </el-select>
          </el-form-item>

          <el-form-item label="收货地址" prop="address">
            <el-input
              v-model="formData.address"
              placeholder="请输入收货地址"
              style="width: 300px"
            />
          </el-form-item>

          <el-form-item label="支付方式" prop="paymentMethod">
            <el-select
              v-model="formData.paymentMethod"
              placeholder="选择支付方式"
              style="width: 300px"
            >
              <el-option label="支付宝" value="alipay" />
              <el-option label="微信" value="wechat" />
              <el-option label="信用卡" value="creditcard" />
            </el-select>
          </el-form-item>

          <el-form-item label="是否购买保险">
            <el-switch
              v-model="formData.insurance"
              active-text="是"
              inactive-text="否"
            />
          </el-form-item>
        </el-form>
      </el-card>

      <el-card>
        <div class="price-summary">
          <div class="price-info">
            <h3>价格明细</h3>
            <p v-if="formData.orderType === '购买'">
              车辆价格：¥ {{ car.purchase_price }}
            </p>
            <p v-if="formData.orderType === 'rent'">
              租赁时长：{{ formData.rentalDuration }} 个月
            </p>
            <p>保险费用：{{ formData.insurance ? "¥ 2000" : "¥ 0" }}</p>
            <p>总金额：¥ {{ totalPrice }}</p>
          </div>

          <div class="order-actions">
            <div class="order-actions1">
              <el-button type="primary" @click="submitOrder">
                提交订单
              </el-button>
              <el-button @click="goBack">返回车辆详情</el-button>
            </div>
            <div class="order-actions2">
              <el-button type="text">联系客服</el-button>
            </div>
          </div>
        </div>
      </el-card>
    </div>
  </div>
</template>

<script>
import { api } from "@/api";
import { mapState } from "vuex";

export default {
  data() {
    return {
      car: { image_paths: [] },
      formData: {
        orderType: "购买",
        rentalDuration: 1,
        insurance: false,
        paymentMethod: "",
        address: "",
      },
      rules: {
        address: [
          { required: true, message: "请输入收货地址", trigger: "blur" },
          { min: 5, message: "地址至少需要 5 个字符", trigger: "blur" },
        ],
        paymentMethod: [
          { required: true, message: "请选择支付方式", trigger: "change" },
        ],
        orderType: [
          { required: true, message: "请选择订单类型", trigger: "change" },
        ],
        rentalDuration: [
          { required: true, message: "请选择租赁时长", trigger: "change" },
        ],
      },
    };
  },
  computed: {
    ...mapState("user", ["userInfo"]),
    totalPrice() {
      let price =
        this.formData.orderType === "购买"
          ? this.car.purchase_price
          : this.car.leasing_price * this.formData.rentalDuration;
      if (this.formData.insurance) price += 2000;
      return price;
    },
  },
  methods: {
    getImageUrl(path) {
      if (!path) return;
      return `http://localhost:8081/image?path=${encodeURIComponent(path)}`;
    },

    async submitOrder() {
      if (this.$store.state.jwt.token == null) {
        this.$message.error("请登录账号后重新提交");
        return;
      }

      if (this.car.available_number == 0) {
        this.$message.error("该车型库存不足，订单提交失败。");
        return;
      }

      if (!this.$refs.orderForm) {
        this.$message.error("表单未加载，请刷新页面");
        return;
      }

      const valid = this.$refs.orderForm.validate();
      if (!valid) {
        this.$message.error("请正确填写订单信息！");
        return;
      }

      const orderData = this.formData;
      orderData.model_id = this.$route.params.id;
      orderData.user_id = this.userInfo.user_id;
      orderData.totalPrice = this.totalPrice;

      console.log(orderData);
      try {
        await api.submitOrder(orderData);
        this.$message.success("提交成功！");
        this.$router.push("/orderlist");
      } catch (error) {
        this.$message.error("提交失败！");
      }
    },

    goBack() {
      if (window.history.length > 1) {
        window.history.back(); // 返回上一页
      } else {
        this.$router.push("/"); // 如果没有上一页，跳转到首页
      }
    },
    async fetchCarDetails() {
      const model_id = this.$route.params.id;
      try {
        const response = await api.getModelById(model_id);
        const { data } = response.data;
        this.car = data;
      } catch (error) {
        console.error("获取车辆信息失败", error);
        this.car = null;
      }
    },
  },
  mounted() {
    this.fetchCarDetails();
  },
};
</script>

<style scoped>
.order-container {
  display: flex;
  gap: 3%;
  padding: 6% 3%;
}

.order-left {
  text-align: center;
  flex: 1.2;
  padding: 20px;
}

.car-image {
  width: 100%;
  object-fit: cover;
  border-radius: 10px;
}

.order-right {
  flex: 1.8;
  display: flex;
  flex-direction: column;
  gap: 40px;
}

.order-options {
  padding: 10px 10%;
  border-radius: 12px;
  box-shadow: 0 4px 10px rgba(0, 0, 0, 0.1);
  display: flex;
  flex-direction: column;
  gap: 20px;
}

.order-title {
  font-size: 20px;
  font-weight: bold;
  margin-bottom: 4%;
}

.price-summary {
  display: flex;
  gap: 15%;
}

.price-info {
  margin-left: 5%;
  width: 30%;
}

.order-actions {
  margin: 5%;
  margin-top: 10%;
  margin-bottom: 0%;
  gap: 10%;
  justify-items: center; /* 水平居中 */
  align-items: center; /* 垂直居中 */
}

.order-actions1 {
  margin-bottom: 5%;
  gap: 10%;
}
</style>
