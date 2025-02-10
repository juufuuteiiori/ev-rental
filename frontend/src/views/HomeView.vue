<template>
  <div>
    <div class="carousel-container">
      <swiper :options="swiperOptions" class="swiper-container">
        <swiper-slide v-for="(image, index) in swiperImages" :key="index">
          <img :src="image.src" :alt="image.alt" class="carousel-image" />
        </swiper-slide>
      </swiper>
    </div>
    <div>
      <div class="section-title">
        <h2 class="title">智能电动车租售服务</h2>
        <h3 class="sub-title">
          灵活的租赁与购车方案，覆盖多种出行场景，让每位用户都能轻松拥有电动车
        </h3>
      </div>

      <div class="content-container">
        <div
          class="content-grid-item"
          v-for="(item, index) in contentItems"
          :key="index"
        >
          <div class="content-image-container">
            <img :src="item.imgSrc" alt="" class="content-image" />
          </div>
          <!-- <p class="content-caption">{{ item.caption }}</p> -->
          <span class="content-short-caption">{{ item.shortCaption }}</span>
          <span class="content-long-caption">{{ item.longCaption }}</span>
        </div>
      </div>

      <div class="button-container">
        <el-button size="medium" @click="handleClick"> 探索心仪座驾 </el-button>
      </div>
    </div>
  </div>
</template>

<script>
import "swiper/css/swiper.css";
import { Swiper, SwiperSlide } from "vue-awesome-swiper"; // 导入 Swiper 和 SwiperSlide

export default {
  name: "HomeView",
  components: {
    Swiper, // 注册 Swiper 组件
    SwiperSlide, // 注册 SwiperSlide 组件
  },
  data() {
    return {
      swiperOptions: {
        pagination: { clickable: true },
        autoplay: { delay: 4000, disableOnInteraction: false },
        loop: true,
      },
      swiperImages: [
        { src: require("@/assets/HomeView/HomePage1.png"), alt: "HomePage 1" },
        { src: require("@/assets/HomeView/HomePage2.png"), alt: "HomePage 2" },
      ],
      contentItems: [
        {
          imgSrc: require("@/assets/HomeView/HomePage1.png"),
          shortCaption: "智能出行体验",
          longCaption: "智能导航与远程控制，让驾驶更便捷",
        },
        {
          imgSrc: require("@/assets/HomeView/HomePage1.png"),
          shortCaption: "高效充电网络",
          longCaption: "覆盖广泛的充电设施，畅享无忧续航",
        },
        {
          imgSrc: require("@/assets/HomeView/HomePage1.png"),
          shortCaption: "灵活租售方案",
          longCaption: "多种购车与租赁模式，满足个性化需求",
        },
        {
          imgSrc: require("@/assets/HomeView/HomePage1.png"),
          shortCaption: "智能安全系统",
          longCaption: "高级驾驶辅助技术，全方位守护安全",
        },
      ],
    };
  },
};
</script>

<style scoped>
.carousel-container {
  width: 100%;
  height: 100vh; /* 让轮播图占满整个屏幕 */
  overflow: hidden;
}

.swiper-container {
  width: 100%;
  height: 100%;
}

.carousel-image {
  width: 100%;
  height: 100%;
  object-fit: cover; /* 确保图片自适应屏幕，不变形 */
}

.section-title {
  justify-content: center; /* 水平居中 */
  align-items: center; /* 垂直居中 */
  margin-top: 30vh; /* 上方留白 */
  margin-bottom: 10vh; /* 下方留白 */
  text-align: center; /* 文字居中 */
}

@import url("https://fonts.googleapis.com/css2?family=Noto+Sans+SC:wght@400;700&display=swap");

.title {
  font-size: clamp(10px, 2.5vw, 35px);
  font-weight: 400;
  letter-spacing: 1.5px;
  color: #333;
  font-family: "PingFang SC", "Noto Sans SC", "Helvetica Neue", Arial,
    sans-serif; /* 设置字体 */
  margin-bottom: 10px;
}

.sub-title {
  font-size: clamp(5px, 1.5vw, 18px);
  color: #666;
  line-height: 1.8;
  max-width: 800px;
  margin: 0 auto;
  margin-top: 3vh;
  letter-spacing: 2px;
  font-family: "PingFang SC", "Noto Sans SC", "Helvetica Neue", Arial,
    sans-serif;
}

.content-container {
  display: grid;
  grid-template-columns: repeat(2, 1fr); /* 设置为两列 */
  grid-gap: 3vw;
  padding-right: 5vw;
  padding-left: 5vw;
  margin: 0 auto; /* 居中显示 */
}

.content-grid-item {
  overflow: hidden; /* 防止内容溢出 */
  text-align: center;
  position: relative; /* 让绝对定位的文字相对于网格项定位 */
}

.content-image-container {
  width: 100%; /* 确保图片容器适应网格 */
  height: 60vh; /* 限制图片容器的高度 */
  overflow: hidden; /* 裁剪超出部分 */
  border-radius: 8px; /* 圆角 */
}

.content-image {
  width: 100%;
  height: 100%;
  object-fit: cover; /* 确保图片不变形，裁剪多余部分 */
  transition: transform 0.3s ease, opacity 0.3s ease;
}

.content-short-caption {
  position: absolute;
  bottom: 20%;
  left: 10%;
  color: white;
  font-size: 1.8vw;
  transition: background 0.3s ease;
}

.content-long-caption {
  position: absolute;
  bottom: 10%;
  left: 10%;
  color: transparent;
  transition: background 0.3s ease;
}

.content-grid-item:hover .content-image {
  transform: scale(1.05);
}

.content-grid-item:hover .content-short-caption {
  font-size: 2vw;
}

.content-grid-item:hover .content-long-caption {
  font-size: 1.5vw;
  color: white;
}

.button-container {
  display: flex;
  justify-content: center;
  align-items: center;
  margin: 10vh 0; /* 控制上下留白 */
}

.el-button {
  font-size: 16px;
  padding: 12px 24px;
  transition: all 0.3s ease;
  border: solid black !important;
  color: black;
}

.el-button:hover {
  transform: scale(1.05);
  background: #409eff !important;
  color: white !important;
}
</style>
