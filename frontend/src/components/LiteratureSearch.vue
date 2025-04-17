<script setup>
import { ref } from 'vue';
import api from '@/api';
import { useRouter } from 'vue-router';


const activeTab = ref('byAuthor');
const router = useRouter();

const authorName = ref('');
const authorResults = ref([]);
const title = ref('');
const titleResults = ref(undefined);
const fuzzyTitle = ref('');
const fuzzyTitleResults = ref([]);
const isLoading = ref(false);

async function handleSearchByAuthorClick() {
    if (!authorName.value.trim()) {
        alert('Please enter an author name');
        return;
    }

    isLoading.value = true;
    try {
        const results = await api.author.getAuthorArticles(authorName.value);
        authorResults.value = results.data;
    } catch (error) {
        console.error('Error fetching author articles:', error);
    } finally {
        isLoading.value = false;
    }
}

async function handleSearchByFuzzyTitleClick() {
    if (!fuzzyTitle.value.trim()) {
        alert('Please enter an fuzzy title');
        return;
    }

    isLoading.value = true;
    try {
        const results = await api.search.searchByTitle(fuzzyTitle.value);
        fuzzyTitleResults.value = results.data;
    } catch (error) {
        console.error('Error fetching fuzzy title articles:', error);
    } finally {
        isLoading.value = false;
    }
}

async function handleSearchByTitleClick() {
    if (!title.value.trim()) {
        alert('Please enter an author name');
        return;
    }

    isLoading.value = true;
    try {
        const results = await api.article.getArticleByTitle(title.value);
        titleResults.value = results.data;
    } catch (error) {
        console.error('Error fetching title articles:', error);
    } finally {
        isLoading.value = false;
    }
}

function viewArticle(articleId) {
    console.log(articleId)
    router.push({ name: 'ArticleDetail', params: { id: articleId } });
}
</script>

<template>
    <div>
        <h2 class="text-2xl font-bold mb-6">Literature Search</h2>

        <div class="tabs tabs-boxed mb-6">
            <a class="tab" :class="{ 'tab-active': activeTab === 'byAuthor' }" @click="activeTab = 'byAuthor'">By
                Author</a>
            <a class="tab" :class="{ 'tab-active': activeTab === 'byTitle' }" @click="activeTab = 'byTitle'">By
                Title</a>
            <a class="tab" :class="{ 'tab-active': activeTab === 'fuzzySearch' }"
                @click="activeTab = 'fuzzySearch'">Fuzzy Search</a>
        </div>

        <!-- Search by Author -->
        <div v-if="activeTab === 'byAuthor'" class="card bg-base-100 shadow-xl">
            <div class="card-body">
                <h2 class="card-title">Search by Author</h2>
                <div class="form-control">
                    <label class="label">
                        <span class="label-text">Author Name</span>
                    </label>
                    <div class="flex gap-2">
                        <input type="text" v-model="authorName" placeholder="Enter author name"
                            class="input input-bordered flex-1" />
                        <button class="btn btn-primary" @click="handleSearchByAuthorClick" :disabled="isLoading">
                            <span v-if="isLoading">Searching...</span>
                            <span v-else>Search</span>
                        </button>
                    </div>
                </div>

                <div class="divider">Results</div>

                <div v-if="isLoading" class="flex justify-center my-4">
                    <span class="loading loading-spinner loading-lg"></span>
                </div>

                <div v-else-if="authorResults.length === 0" class="text-center py-4 text-gray-500">
                    No results found
                </div>

                <div v-else class="overflow-x-auto">
                    <table class="table">
                        <thead>
                            <tr>
                                <th>Title</th>
                                <th>Year</th>
                                <th>Authors</th>
                                <th>Actions</th>
                            </tr>
                        </thead>
                        <tbody>
                            <tr v-for="article in authorResults" :key="article.article_id">
                                <td>{{ article.title }}</td>
                                <td>{{ article.year }}</td>
                                <td>{{ article.authors ? article.authors.join(', ') : "" }}</td>
                                <td>
                                    <button class="btn btn-xs btn-ghost"
                                        @click="viewArticle(article.article_id)">View</button>
                                </td>
                            </tr>
                        </tbody>
                    </table>
                </div>
            </div>
        </div>


        <!-- Search by Title -->
        <div v-if="activeTab === 'byTitle'" class="card bg-base-100 shadow-xl">
            <div class="card-body">
                <h2 class="card-title">Search by Title</h2>
                <div class="form-control">
                    <label class="label">
                        <span class="label-text">Paper Title</span>
                    </label>
                    <div class="flex gap-2">
                        <input type="text" placeholder="Enter paper title" v-model="title"
                            class="input input-bordered flex-1" />
                        <button class="btn btn-primary" @click="handleSearchByTitleClick">Search</button>
                    </div>
                </div>

                <div class="divider">Results</div>

                <div v-if="isLoading" class="flex justify-center my-4">
                    <span class="loading loading-spinner loading-lg"></span>
                </div>

                <div v-else-if="!titleResults" class="text-center py-4 text-gray-500">
                    No results found
                </div>

                <div v-else class="overflow-x-auto">
                    <div class="card bg-base-200">
                        <div class="card-body">
                            <h2 class="card-title">{{ titleResults.title }}</h2>
                            <p>Year: {{ titleResults.year }}</p>
                            <p>Authors: {{ titleResults.authors.join(", ") + "." }}</p>
                            <p>Keywords: {{ titleResults.keywords.join(", ") }}</p>
                            <div class="flex justify-end">
                                <button class="btn btn-neutral btn-secondary w-32"
                                    @click="viewArticle(titleResults.article_id)">View
                                    Details</button>
                            </div>
                        </div>
                    </div>
                </div>
            </div>
        </div>


        <!-- Fuzzy Search -->
        <div v-if="activeTab === 'fuzzySearch'" class="card bg-base-100 shadow-xl">
            <div class="card-body">
                <h2 class="card-title">Fuzzy Title Search</h2>
                <div class="form-control">
                    <label class="label">
                        <span class="label-text">Title Keywords</span>
                    </label>
                    <div class="flex gap-2">
                        <input type="text" v-model="fuzzyTitle" placeholder="Enter title keywords"
                            class="input input-bordered flex-1" />
                        <button class="btn btn-primary" @click="handleSearchByFuzzyTitleClick">Search</button>
                    </div>
                </div>

                <div class="divider">Results</div>

                <div v-if="isLoading" class="flex justify-center my-4">
                    <span class="loading loading-spinner loading-lg"></span>
                </div>

                <div v-else-if="fuzzyTitleResults.length === 0" class="text-center py-4 text-gray-500">
                    No results found
                </div>

                <div v-else class="overflow-x-auto">
                    <table class="table">
                        <thead>
                            <tr>
                                <th>Title</th>
                                <th>Year</th>
                                <th>Authors</th>
                                <th>Actions</th>
                            </tr>
                        </thead>
                        <tbody>
                            <tr v-for="article in fuzzyTitleResults" :key="article.article_id">
                                <td>{{ article.title }}</td>
                                <td>{{ article.year }}</td>
                                <td>{{ article.authors ? article.authors.join(', ') : "" }}</td>
                                <td>
                                    <button class="btn btn-xs btn-ghost"
                                        @click="viewArticle(article.article_id)">View</button>
                                </td>
                            </tr>
                        </tbody>
                    </table>
                </div>
            </div>
        </div>
    </div>
</template>
