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
        <el-button type="primary" @click="submitComment(newPost, 0)"
          >发布</el-button
        >
      </div>
    </el-card>

    <!-- 帖子列表 -->
    <el-card v-for="post in posts" :key="post.id" class="post-card">
      <h3>{{ post.author }}</h3>
      <p>{{ post.content }}</p>
      <div class="post-footer">
        <span>{{ post.date }}</span>
        <el-button @click="likePost(post.id)" icon="el-icon-thumb">
          {{ post.likes }}
        </el-button>

        <div>
          <el-button
            type="danger"
            v-if="isAuth(post.id)"
            @click="delComment(post.id)"
          >
            删除评论
          </el-button>
          <el-button type="primary" @click="toggleComments(post.id)">
            {{ post.showComments ? "关闭评论" : "打开评论" }}
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
          <el-button
            type="primary"
            @click="submitComment(post.newComment, post.id)"
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
  </div>
</template>

<script>
import { api } from "@/api";

export default {
  data() {
    return {
      newPost: { content: "", comment_id: 0 },
      posts: [],
    };
  },

  created() {
    this.fetchComments();
  },

  methods: {
    async fetchComments() {
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

    async toggleComments(postId) {
      const post = this.posts.find((p) => p.id === postId);
      if (post) {
        post.showComments = !post.showComments;
        try {
          if (post.showComments) {
            const response = await api.getCommentsById(postId);
            this.$set(post, "comments", response.data.comments);
          }
        } catch (error) {
          this.$message.error("服务器错误");
        }
      }
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
</style>
