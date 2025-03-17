<template>
  <div class="discussion-container">
    <!-- 发帖表单 -->
    <el-card class="post-form">
      <el-input
        v-model="newPost.content"
        placeholder="分享你的想法..."
        type="textarea"
        rows="5"
      ></el-input>

      <div class="button-container">
        <el-button type="primary" @click="handleSubmitComment()"
          >发布</el-button
        >
      </div>
    </el-card>

    <el-card class="filter-bar">
      <el-row :gutter="24">
        <el-col :span="10">
          <el-date-picker
            v-model="filters.dateRange"
            type="daterange"
            range-separator="至"
            start-placeholder="开始日期"
            end-placeholder="结束日期"
            format="yyyy-MM-dd"
            value-format="yyyy-MM-dd"
            clearable
            class="custom-select"
          />
        </el-col>

        <el-col :span="8">
          <el-input
            v-model="filters.keyword"
            placeholder="请输入用户名或关键词"
            clearable
            class="custom-input"
          >
            <template #append>
              <el-button icon="el-icon-search" />
            </template>
          </el-input>
        </el-col>
      </el-row>
    </el-card>

    <!-- 帖子列表 -->
    <el-card
      v-for="post in paginatedPosts"
      :key="post.id"
      class="post-card"
      ref="contentCard"
    >
      <h3>{{ post.author }}</h3>
      <p>{{ post.content }}</p>
      <div class="post-footer">
        <div style="width: 33%">
          <span>{{ post.date }}</span>
        </div>
        <div style="width: 33%; text-align: center">
          <el-button
            @click="likePost(post.id)"
            :type="post.is_liked ? 'primary' : 'default'"
          >
            点赞：
            {{ post.like_num }}
          </el-button>
          <el-button
            @click="dislikePost(post.id)"
            :type="post.is_disliked ? 'danger' : 'default'"
          >
            不喜欢：
            {{ post.dislike_num }}
          </el-button>
        </div>

        <div style="width: 33%; text-align: right">
          <el-button
            type="danger"
            v-if="isAuth(post.id)"
            @click="delComment(post.id)"
          >
            删除评论
          </el-button>
          <el-button type="primary" @click="toggleComments(post.id)">
            {{ post.showComments ? "关闭回复" : "打开回复" }}
          </el-button>
        </div>
      </div>

      <!-- 评论区 -->
      <div v-if="post.showComments" class="comment-container">
        <el-input
          v-model="post.newComment.content"
          placeholder="写下你的评论..."
          type="textarea"
          rows="3"
        />

        <div style="text-align: right; margin: 10px">
          <el-button type="primary" @click="handleSubmitComment(post)"
            >发表评论</el-button
          >
        </div>

        <el-card
          class="comment-card"
          v-for="comment in post.comments"
          :key="comment.id"
        >
          <div class="comment-header">
            <span class="comment-author">{{ comment.author }}</span>
            <span class="comment-time">{{ comment.date }}</span>
          </div>
          <p class="comment-content">{{ comment.content }}</p>
        </el-card>
      </div>
    </el-card>

    <!-- 分页 -->
    <div class="paginated-container">
      <el-pagination
        background
        layout="prev, pager, next"
        :total="filteredPosts.length"
        :page-size="pageSize"
        :current-page="currentPage"
        @current-change="changePage"
      />
    </div>
  </div>
</template>

<script>
import { api } from "@/api";

export default {
  data() {
    return {
      // 筛选项
      filters: {
        dateRange: null,
        keyword: "",
      },

      newPost: { content: "", comment_id: 0 },
      posts: [],

      // 分页显示
      currentPage: 1,
      pageSize: 5,
    };
  },

  computed: {
    filteredPosts() {
      return this.posts
        .filter(
          (post) =>
            !this.filters.dateRange ||
            (new Date(this.filters.dateRange[0]) <= new Date(post.date) &&
              new Date(post.date) <= new Date(this.filters.dateRange[1]))
        )
        .filter(
          (post) =>
            !this.filters.keyword ||
            post.content.includes(this.filters.keyword) ||
            post.author.includes(this.filters.keyword)
        );
    },

    paginatedPosts() {
      return this.filteredPosts.slice(
        (this.currentPage - 1) * this.pageSize,
        this.currentPage * this.pageSize
      );
    },
  },

  created() {
    this.fetchComments();
  },

  methods: {
    async fetchComments() {
      if (this.$store.state.user.userInfo.user_id == null) {
        this.$message.error("请先登录");
        return;
      }

      try {
        const response = await api.getComments();
        this.posts = response.data.comments.map((post) => ({
          ...post,
          showComments: false,
          newComment: { content: "", parent_id: 0 },
        }));
      } catch (error) {
        this.$message.error("服务器错误");
      }
    },

    isAuth(postId) {
      const post = this.posts.find((p) => p.id === postId);
      return (
        post.user_id == this.$store.state.user.userInfo.user_id ||
        this.$store.state.user.userInfo.role === "管理员"
      );
    },

    handleSubmitComment(post = null) {
      if (post) {
        this.submitComment(post.newComment, post.id);
        this.$nextTick(() => {
          post.newComment.content = "";
          this.fetchComments();
        });
      } else {
        this.submitComment(this.newPost, 0);
        this.$nextTick(() => {
          this.newPost.content = "";
        });
      }
    },

    async submitComment(newPost, parent_id) {
      if (!newPost.content.trim()) {
        this.$message.warning("请输入内容");
        return;
      }

      newPost.user_id = this.$store.state.user.userInfo.user_id;
      if (this.newPost.user_id == 0) {
        this.$message.error("请先登录");
        return;
      }
      if (parent_id != 0) newPost.parent_id = parent_id;

      try {
        const response = await api.addComment(newPost);
        this.$message.success(response.data.msg);
      } catch (error) {
        this.$message.error("发布失败");
      }
    },

    async delComment(postId) {
      try {
        const response = await api.delComment(postId);
        this.$message.success(response.data.msg);
      } catch (error) {
        this.$message.error("删除失败");
      }
    },

    toggleComments(postId) {
      const post = this.posts.find((p) => p.id === postId);
      post.showComments = !post.showComments;
      this.fetchCommentsById(postId);
    },

    async likePost(postId) {
      const post = this.posts.find((p) => p.id === postId);
      if (!post) return;
      try {
        if (post.is_liked) {
          const response = await api.delLike(postId);
          this.$message.success(response.data.msg);
          post.like_num -= 1;
        } else {
          const response = await api.addLike(postId);
          this.$message.success(response.data.msg);
          post.like_num += 1;
        }

        post.is_liked = !post.is_liked;
      } catch (error) {
        this.$message.error("操作失败");
      }
    },

    async dislikePost(postId) {
      const post = this.posts.find((p) => p.id === postId);
      if (!post) return;
      try {
        if (post.is_disliked) {
          const response = await api.delDislike(postId);
          this.$message.success(response.data.msg);
          post.dislike_num -= 1;
        } else {
          const response = await api.addDislike(postId);
          this.$message.success(response.data.msg);
          post.dislike_num += 1;
        }

        post.is_disliked = !post.is_disliked;
      } catch (error) {
        this.$message.error("操作失败");
      }
    },

    async fetchCommentsById(postId) {
      const post = this.posts.find((p) => p.id === postId);
      try {
        if (post.showComments) {
          const response = await api.getCommentsById(postId);
          this.$set(post, "comments", response.data.comments);
        }
      } catch (error) {
        this.$message.error("服务器错误");
      }
    },

    // 改变分页
    changePage(page) {
      this.currentPage = page;
      window.scrollTo({ top: window.innerHeight * 0.3, behavior: "smooth" });
    },

    // likePost(postId) {
    //   const post = this.posts.find((p) => p.id === postId);
    //   if (post) post.likes++;
    // },
  },
};
</script>

<style scoped>
.discussion-container {
  margin: 5%;
  padding: 0 10%;
}

.post-form {
  display: flex;
  flex-direction: column;
  padding: 15px;
  margin-bottom: 2%;
  position: relative; /* 允许子元素绝对定位 */
}

.button-container {
  display: flex;
  justify-content: flex-end; /* 让按钮靠右 */
  margin-top: 10px; /* 增加按钮与输入框的间距 */
}

.post-card {
  margin-bottom: 10px;
}

.post-footer {
  display: flex;
  justify-content: space-between;
  align-items: center;
}

.comment-container {
  margin: 2%;
}

.comment-card {
  margin: 5px 0;
  padding: 10px;
  border-radius: 8px;
}

.comment-header {
  display: flex;
  justify-content: space-between;
  font-size: 14px;
  font-weight: bold;
}

.comment-content {
  margin-top: 5px;
  font-size: 14px;
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

.filter-bar {
  background-color: #f8f9fa; /* 浅灰色背景 */
  margin: 5px 0 5px 0;
  padding: 15px;
  padding-left: 3vw;
  box-shadow: 0 4px 10px rgba(0, 0, 0, 0.05); /* 轻微阴影 */
  transition: all 0.3s ease-in-out;
}

.custom-input {
  width: 100%;
  border-radius: 8px; /* 让输入框更圆润 */
}

/* 选中时的高亮效果 */
.custom-input :deep(.el-input__inner) {
  transition: all 0.3s ease-in-out;
  border-radius: 8px;
}

.custom-input :deep(.el-input__inner):hover {
  border-color: #409eff; /* Element-UI 主题色 */
}

.custom-input :deep(.el-input__inner):focus {
  border-color: #007bff; /* 选中时颜色更深 */
  box-shadow: 0 0 8px rgba(64, 158, 255, 0.3);
}
</style>
