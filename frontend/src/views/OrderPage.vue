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
        <h3 class="order-title">订单信息</h3>

        <div class="order-radio">
          <el-radio v-model="orderType" label="buy" border size="medium"
            >购买</el-radio
          >
          <el-radio v-model="orderType" label="rent" border size="medium"
            >租赁</el-radio
          >
          <el-select
            v-if="orderType === 'rent'"
            v-model="rentalDuration"
            placeholder="选择租赁时长"
          >
            <el-option label="1 个月" :value="1" />
            <el-option label="3 个月" :value="3" />
            <el-option label="6 个月" :value="6" />
            <el-option label="12 个月" :value="12" />
          </el-select>
        </div>

        <div class="order-checkbox">
          <p>是否购买保险</p>
          <el-switch v-model="insurance" active-text="是" inactive-text="否">
          </el-switch>
        </div>

        <el-input
          v-model="address"
          placeholder="请输入收货地址"
          class="order-input"
        ></el-input>

        <el-select
          v-model="paymentMethod"
          placeholder="选择支付方式"
          class="order-select"
        >
          <el-option label="支付宝" value="alipay" />
          <el-option label="微信" value="wechat" />
          <el-option label="信用卡" value="creditcard" />
        </el-select>
      </el-card>

      <el-card>
        <div class="price-summary">
          <div class="price-info">
            <h3>价格明细</h3>
            <p v-if="orderType === 'buy'">
              车辆价格：¥ {{ car.purchase_price }}
            </p>
            <p v-if="orderType === 'rent'">
              租赁时长：{{ rentalDuration }} 个月
            </p>
            <p>保险费用：{{ insurance ? "¥ 2000" : "¥ 0" }}</p>
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

export default {
  props: ["carId"],
  data() {
    return {
      car: {}, // 车辆数据
      orderType: "buy",
      rentalDuration: 1,
      insurance: false,
      paymentMethod: "",
      address: "",
    };
  },
  computed: {
    totalPrice() {
      let price =
        this.orderType === "buy"
          ? this.car.purchase_price
          : this.car.leasing_price * this.rentalDuration;
      if (this.insurance) price += 2000;
      return price;
    },
  },
  methods: {
    getImageUrl(imagePath) {
      return `http://localhost:8081/static/${imagePath}`;
    },
    submitOrder() {
      if (this.car.available_number == 0) {
        this.$message.error("该车型库存不足，订单提交失败。");
      } else {
        this.$message.success("订单已提交！");
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
      try {
        const response = await api.getModelById(this.carId);
        const { data } = response.data;
        this.car = data;
      } catch (error) {
        console.error("获取车辆信息失败", error);
        this.car = null;
      }
    },
  },
  mounted() {
    console.log("当前下单车辆ID:", this.carId);
    this.fetchCarDetails();
  },
};
</script>

<style scoped>
.order-container {
  display: flex;
  gap: 3%;
  padding: 5% 3%;
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
  gap: 20px;
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

.order-radio {
  display: flex;
  margin-top: 20px;
  gap: 8%;
  width: 80%;
  min-height: 6vh;
}

.order-checkbox {
  display: flex;
  align-items: center; /* 让内容垂直居中 */
  gap: 20px; /* 控制文字和开关之间的间距 */
}

.order-input,
.order-select {
  width: 80%;
  height: 36px; /* 统一高度，增强视觉一致性 */
  margin-bottom: 30px; /* 增加间距，避免元素贴在一起 */
}

/* 鼠标悬浮和输入时的效果 */
.order-input:hover,
.order-input:focus,
.order-select:hover,
.order-select:focus {
  border-color: #409eff; /* Element UI 主题色 */
  box-shadow: 0 0 8px rgba(64, 158, 255, 0.2);
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
