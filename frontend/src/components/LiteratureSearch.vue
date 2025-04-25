<script setup>
import api from '@/api';
import { useRouter, useRoute } from 'vue-router';
import { ref, onMounted, onUnmounted, watch, computed, nextTick } from 'vue';
import { ChevronDoubleLeftIcon, ChevronDoubleRightIcon } from '@heroicons/vue/16/solid';
import { useToast } from 'vue-toastification';

const toast = useToast();
const router = useRouter();
const route = useRoute();

const activeTab = ref('byAuthor');
const authorName = ref('');
const authorResults = ref([]);
const title = ref('');
const titleResults = ref(undefined);
const fuzzyTitle = ref('');
const fuzzyTitleResults = ref([]);
const isLoading = ref(false);

// Pagination for fuzzy search
const currentPage = ref(1);
const itemsPerPage = 50;

onMounted(() => {
    // Restore tab if specified in the URL
    if (route.query.tab) {
        activeTab.value = route.query.tab;
    }

    // Restore search parameters if available
    if (route.query.authorName) {
        authorName.value = route.query.authorName;
        if (activeTab.value === 'byAuthor') {
            handleSearchByAuthorClick();
        }
    }

    if (route.query.title) {
        title.value = route.query.title;
        if (activeTab.value === 'byTitle') {
            handleSearchByTitleClick();
        }
    }

    if (route.query.fuzzyTitle) {
        fuzzyTitle.value = route.query.fuzzyTitle;
        if (activeTab.value === 'fuzzySearch') {
            handleSearchByFuzzyTitleClick();
            if (route.query.page) {
                currentPage.value = parseInt(route.query.page) || 1;
            }
        }
    }
});

// Update URL when tab changes to maintain state
watch(activeTab, (newTab) => {
    router.replace({ query: { ...route.query, tab: newTab } });
});

const centerActiveButton = () => {
    nextTick(() => {
        setTimeout(() => {
            const activeButton = document.querySelector('.pagination-button.btn-active');
            if (!activeButton) return;

            const container = document.querySelector('.pagination-buttons-container');
            if (!container) return;

            const containerWidth = container.clientWidth;
            const allButtons = Array.from(document.querySelectorAll('.pagination-button'));
            const activeIndex = allButtons.indexOf(activeButton);

            let totalWidth = 0;
            let targetPosition = 0;

            for (let i = 0; i < allButtons.length; i++) {
                const btn = allButtons[i];
                if (i < activeIndex) {
                    totalWidth += btn.offsetWidth;
                } else if (i === activeIndex) {
                    targetPosition = totalWidth + (btn.offsetWidth / 2) - (containerWidth / 2);
                    break;
                }
            }

            targetPosition = Math.max(0, targetPosition);

            container.scrollTo({
                left: targetPosition,
                behavior: 'smooth'
            });
        }, 50);
    });
};

watch(currentPage, () => {
    centerActiveButton();
});

const debounce = (fn, delay) => {
    let timer = null;
    return function (...args) {
        if (timer) clearTimeout(timer);
        timer = setTimeout(() => {
            fn.apply(this, args);
        }, delay);
    };
};

const handleResize = debounce(() => {
    centerActiveButton();
}, 100);

onMounted(() => {
    window.addEventListener('resize', handleResize);

    centerActiveButton();
});

onUnmounted(() => {
    window.removeEventListener('resize', handleResize);
});


async function handleSearchByAuthorClick() {
    if (!authorName.value.trim()) {
        toast.error(`Empty author name!`);
        return;
    }

    isLoading.value = true;
    try {
        const results = await api.author.getAuthorArticles(authorName.value);
        authorResults.value = results.data;
        toast.success("Author's articles fetched successfully!");
    } catch (error) {
        toast.error(`Error fetching author's articles: ${error}`);
    } finally {
        isLoading.value = false;
    }
}

async function handleSearchByFuzzyTitleClick() {
    if (!fuzzyTitle.value.trim()) {
        toast.error(`Empty keywords!`);
        return;
    }

    isLoading.value = true;
    try {
        const results = await api.search.searchByTitle(fuzzyTitle.value);
        fuzzyTitleResults.value = results.data;
        currentPage.value = 1;
        toast.success("Articles with keywords fetched successfully!");
    } catch (error) {
        toast.error(`Error fetching articles with keywords: ${error}`);
    } finally {
        isLoading.value = false;
    }
}

const totalPages = computed(() => {
    if (!fuzzyTitleResults.value || fuzzyTitleResults.value.length === 0) return 0;
    return Math.ceil(fuzzyTitleResults.value.length / itemsPerPage);
});

const paginatedFuzzyResults = computed(() => {
    if (!fuzzyTitleResults.value || fuzzyTitleResults.value.length === 0) return [];

    const start = (currentPage.value - 1) * itemsPerPage;
    const end = start + itemsPerPage;
    return fuzzyTitleResults.value.slice(start, end);
});

watch(totalPages, (newTotal) => {
    if (currentPage.value > newTotal && newTotal > 0) {
        currentPage.value = 1;
    }
});


async function handleSearchByTitleClick() {
    if (!title.value.trim()) {
        toast.error(`Empty title!`);
        return;
    }

    isLoading.value = true;
    try {
        const results = await api.article.getArticleByTitle(title.value);
        titleResults.value = results.data;
        toast.success("Articles of title fetched successfully!");
    } catch (error) {
        toast.error(`Error fetching article of title: ${error}`);
    } finally {
        isLoading.value = false;
    }
}

function viewArticle(articleId) {
    // Store current state in the route when navigating to article detail
    let queryParams = {
        returnPage: "LiteratureSearch",
        returnTab: activeTab.value
    };

    // Add relevant search parameters based on current tab
    if (activeTab.value === 'byAuthor' && authorName.value) {
        queryParams.authorName = authorName.value;
    } else if (activeTab.value === 'byTitle' && title.value) {
        queryParams.title = title.value;
    } else if (activeTab.value === 'fuzzySearch' && fuzzyTitle.value) {
        queryParams.fuzzyTitle = fuzzyTitle.value;
        queryParams.page = currentPage.value;
    }

    router.push({
        name: 'ArticleDetail',
        params: { id: articleId },
        query: queryParams
    });
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
                            <tr v-for="article in paginatedFuzzyResults" :key="article.article_id">
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

                    <div v-if="fuzzyTitleResults.length > 0" class="mt-4 flex justify-center">
                        <div class="flex join pagination-container">
                            <button class="join-item btn pagination-prev" :class="{ 'btn-disabled': currentPage === 1 }"
                                @click="currentPage--" :disabled="currentPage === 1">
                                <ChevronDoubleLeftIcon class="size-4" />
                            </button>

                            <div class="join-item overflow-x-auto max-w-[50vw] flex pagination-buttons-container"
                                style="scrollbar-width: none;">
                                <div class="join flex">
                                    <button v-for="page in totalPages" :key="page"
                                        class="join-item btn pagination-button"
                                        :class="{ 'btn-active': currentPage === page }" @click="currentPage = page">
                                        {{ page }}
                                    </button>
                                </div>
                            </div>

                            <button class="join-item btn pagination-next"
                                :class="{ 'btn-disabled': currentPage === totalPages }" @click="currentPage++"
                                :disabled="currentPage === totalPages">
                                <ChevronDoubleRightIcon class="size-4" />
                            </button>
                        </div>
                    </div>

                </div>
            </div>
        </div>
    </div>
</template>
