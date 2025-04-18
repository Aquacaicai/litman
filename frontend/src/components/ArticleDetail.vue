<script setup>
import { XCircleIcon, ExclamationTriangleIcon, ArrowUturnLeftIcon } from '@heroicons/vue/24/outline'
import { ref, onMounted } from 'vue';
import { useRoute, useRouter } from 'vue-router';
import api from '@/api';

const route = useRoute();
const router = useRouter();
const article = ref(null);
const isLoading = ref(true);
const error = ref(null);

onMounted(async () => {
  const articleId = route.params.id;
  if (!articleId) {
    error.value = 'No article ID provided';
    isLoading.value = false;
    return;
  }

  try {
    const response = await api.article.getArticle(articleId);
    article.value = response.data;
  } catch (err) {
    console.error('Error fetching article:', err);
    error.value = 'Failed to load article details. Please try again later.';
  } finally {
    isLoading.value = false;
  }
});

function goBack() {
  if (route.query.returnPage) {
    if (route.query.returnPage === 'LiteratureSearch') {
      router.push({
        name: 'LiteratureSearch',
        query: {
          tab: route.query.returnTab,
          authorName: route.query.authorName || '',
          title: route.query.title || '',
          fuzzyTitle: route.query.fuzzyTitle || ''
        }
      })
    } else if (route.query.returnPage === 'Collaborations') {
      router.push({
        name: 'Collaborations',
        query: {
          tab: route.query.returnTab,
          coauthor: route.query.coauthor || '',
          authorName: route.query.authorName || '',
        }
      })
    }
  } else {
    router.back();
  }
}
</script>

<template>
  <div class="container mx-auto p-4">
    <button @click="goBack" class="btn btn-ghost mb-4">
      <ArrowUturnLeftIcon class="size-5" />
      Back
    </button>

    <div v-if="isLoading" class="flex justify-center my-8">
      <span class="loading loading-spinner loading-lg"></span>
    </div>

    <div v-else-if="error" class="alert alert-error shadow-lg">
      <div>
        <XCircleIcon class="size-6" />
        <span>{{ error }}</span>
      </div>
    </div>

    <div v-else-if="article" class="card bg-base-100 shadow-xl">
      <div class="card-body">
        <h1 class="card-title text-2xl">{{ article.title }}</h1>

        <div class="grid grid-cols-1 md:grid-cols-2 gap-4 mt-4">
          <div class="col-span-2">
            <div class="badge badge-primary mr-2" v-if="article.year">{{ article.year }}</div>
            <div class="badge badge-secondary mr-2" v-if="article.journal">{{ article.journal }}</div>
            <div class="badge badge-accent" v-if="article.booktitle">{{ article.booktitle }}</div>
          </div>

          <div class="mb-4" v-if="article.authors && article.authors.length">
            <h3 class="font-bold text-lg">Authors</h3>
            <p>{{ article.authors.join(", ") }}</p>
          </div>

          <div class="mb-4" v-if="article.editors && article.editors.length">
            <h3 class="font-bold text-lg">Editors</h3>
            <p>{{ article.editors.join(", ") }}</p>
          </div>

          <div class="mb-4" v-if="article.keywords && article.keywords.length">
            <h3 class="font-bold text-lg">Keywords</h3>
            <div class="flex flex-wrap gap-1 mt-1">
              <div class="badge badge-outline" v-for="(keyword, index) in article.keywords" :key="index">
                {{ keyword }}
              </div>
            </div>
          </div>

          <div class="mb-4" v-if="article.publisher">
            <h3 class="font-bold text-lg">Publisher</h3>
            <p>{{ article.publisher }}</p>
          </div>

          <div class="col-span-2">
            <div class="divider"></div>
            <div class="grid grid-cols-1 md:grid-cols-3 gap-4">
              <div v-if="article.volume">
                <h3 class="font-bold">Volume</h3>
                <p>{{ article.volume }}</p>
              </div>

              <div v-if="article.series">
                <h3 class="font-bold">Series</h3>
                <p>{{ article.series }}</p>
              </div>

              <div v-if="article.pages">
                <h3 class="font-bold">Pages</h3>
                <p>{{ article.pages }}</p>
              </div>

              <div v-if="article.isbn">
                <h3 class="font-bold">ISBN</h3>
                <p>{{ article.isbn }}</p>
              </div>

              <div v-if="article.school">
                <h3 class="font-bold">School</h3>
                <p>{{ article.school }}</p>
              </div>
            </div>
          </div>
        </div>

        <div class="card-actions justify-end mt-6">
          <a v-if="article.url" :href="`https://dblp.org/${article.url}`" target="_blank" class="btn btn-primary">
            Visit Source
          </a>
          <a v-if="article.ee" :href="article.ee" target="_blank" class="btn btn-outline">
            Electronic Edition
          </a>
        </div>
      </div>
    </div>

    <div v-else class="alert alert-warning shadow-lg">
      <div>
        <ExclamationTriangleIcon class="size-6" />
        <span>No article found.</span>
      </div>
    </div>
  </div>
</template>