<template>
  <div id="app">
    <NavBar />
    <router-view v-slot="{ Component }">
      <keep-alive>
        <component :is="Component" />
      </keep-alive>
    </router-view>
    <FooterBar />
  </div>
</template>

<script>
import NavBar from "@/components/NavBar.vue";
import FooterBar from "@/components/FooterBar.vue";

export default {
  name: "App",
  components: {
    NavBar,
    FooterBar,
  },
};
</script>

<style>
body {
  margin: 0;
  padding: 0;
  height: 100%;
}
</style>
