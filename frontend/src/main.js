import Vue from 'vue'
import HomeView from './views/HomeView.vue'
import router from './router'  // 引入路由配置
import ElementUI from 'element-ui'
import 'element-ui/lib/theme-chalk/index.css'

Vue.use(ElementUI)

Vue.config.productionTip = false

new Vue({
  router, // 挂载路由
  render: h => h(HomeView)
}).$mount('#app')
