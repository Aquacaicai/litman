<script setup>
import { ref, watch, computed } from 'vue';
import { ChevronDoubleLeftIcon, ChevronDoubleRightIcon } from '@heroicons/vue/16/solid'
import api from '@/api';
import { useRouter } from 'vue-router';
import { use } from 'echarts/core';
import { CanvasRenderer } from 'echarts/renderers';
import { BarChart } from 'echarts/charts';
import { GridComponent, TooltipComponent, DataZoomComponent } from 'echarts/components';
import VChart from 'vue-echarts';

// Register ECharts components
use([
    CanvasRenderer,
    BarChart,
    GridComponent,
    TooltipComponent,
    DataZoomComponent
]);

const router = useRouter();
const activeTab = ref('authorStats');
const authorFilter = ref('');

const authorStats = ref(undefined);
const currentPage = ref(1);
const itemsPerPage = 10;

const authorChartOptions = computed(() => {
    if (!authorStats.value) return { series: [] };

    const topAuthors = [...authorStats.value]
        .sort((a, b) => a.count - b.count)

    return {
        tooltip: {
            trigger: 'axis',
            axisPointer: {
                type: 'shadow'
            }
        },
        grid: {
            left: '3%',
            right: '4%',
            bottom: '3%',
            containLabel: true
        },
        xAxis: {
            type: 'value',
            boundaryGap: [0, 0.01]
        },
        yAxis: {
            type: 'category',
            data: topAuthors.map(a => a.author),
            axisLabel: {
                width: 120,
                overflow: 'truncate'
            }
        },
        dataZoom: [
            {
                type: 'slider',
                yAxisIndex: 0,
                start: 90,
                end: 99
            },
            {
                type: 'inside',
                yAxisIndex: 0,
                zoomOnMouseWheel: true,
            }
        ],
        series: [
            {
                name: 'Publications',
                type: 'bar',
                data: topAuthors.map(a => a.count),
                itemStyle: {
                    color: '#3b82f6'
                }
            }
        ]
    };
});

const filteredAuthors = computed(() => {
    if (!authorStats.value) return [];

    if (!authorFilter.value.trim()) {
        return authorStats.value;
    }

    const searchTerm = authorFilter.value.toLowerCase().trim();
    return authorStats.value.filter(stat =>
        stat.author.toLowerCase().includes(searchTerm)
    );
});

const totalPages = computed(() => {
    if (!filteredAuthors.value) return 0;
    return Math.ceil(filteredAuthors.value.length / itemsPerPage);
});

const paginatedAuthors = computed(() => {
    if (!filteredAuthors.value) return [];

    const start = (currentPage.value - 1) * itemsPerPage;
    const end = start + itemsPerPage;
    return filteredAuthors.value.slice(start, end);
});

async function loadAuthorStatistics() {
    let data = await api.stats.getAuthorArticleCounts();
    authorStats.value = data.data;
}

function loadKeywordTrends() {
}

function viewAuthorPapers(authorName) {
    // Navigate to LiteratureSearch with author name pre-filled
    router.push({
        name: 'LiteratureSearch',
        query: {
            tab: 'byAuthor',
            authorName
        }
    });
}

watch(authorFilter, () => {
    currentPage.value = 1;
});

watch(totalPages, (newTotal) => {
    if (currentPage.value > newTotal && newTotal > 0) {
        currentPage.value = 1;
    }
});

watch(activeTab, (newValue, oldValue) => {
    if (newValue === 'authorStats') {
        loadAuthorStatistics();
    } else if (newValue === 'keywordStats') {
        loadKeywordTrends();
    }
}, { immediate: true });
</script>


<template>
    <div>
        <h2 class="text-2xl font-bold mb-6">Statistics</h2>

        <div class="tabs tabs-boxed mb-6">
            <a class="tab" :class="{ 'tab-active': activeTab === 'authorStats' }"
                @click="activeTab = 'authorStats'">Author Statistics</a>
            <a class="tab" :class="{ 'tab-active': activeTab === 'keywordStats' }"
                @click="activeTab = 'keywordStats'">Keyword Trends</a>
        </div>

        <!-- Author Statistics -->
        <div v-if="activeTab === 'authorStats'" class="card bg-base-100 shadow-xl">
            <div class="card-body">
                <h2 class="card-title">Publications by Author</h2>

                <div class="form-control mb-4 h-12">
                    <input type="text" placeholder="Filter authors" v-model="authorFilter"
                        class="input input-bordered flex-1" />
                </div>

                <div class="overflow-x-auto">
                    <table class="table">
                        <thead>
                            <tr>
                                <th>Rank</th>
                                <th>Author</th>
                                <th>Publications</th>
                                <th>Actions</th>
                            </tr>
                        </thead>
                        <tbody>
                            <tr v-for="[index, stat] in paginatedAuthors.entries()">
                                <td>{{ (currentPage - 1) * itemsPerPage + index + 1 }}</td>
                                <td>{{ stat.author }}</td>
                                <td>{{ stat.count }}</td>
                                <td>
                                    <button class="btn btn-xs btn-ghost" @click="viewAuthorPapers(stat.author)">View
                                        Papers</button>
                                </td>
                            </tr>
                        </tbody>
                    </table>

                    <div class="join mt-4 flex justify-center">
                        <button class="join-item btn" :class="{ 'btn-disabled': currentPage === 1 }"
                            @click="currentPage--" :disabled="currentPage === 1">
                            <ChevronDoubleLeftIcon class="size-4" />
                        </button>

                        <button v-for="page in totalPages" :key="page" class="join-item btn"
                            :class="{ 'btn-active': currentPage === page }" @click="currentPage = page">
                            {{ page }}
                        </button>

                        <button class="join-item btn" :class="{ 'btn-disabled': currentPage === totalPages }"
                            @click="currentPage++" :disabled="currentPage === totalPages">
                            <ChevronDoubleRightIcon class="size-4" />
                        </button>
                    </div>
                </div>

                <div class="w-full h-80">
                    <v-chart class="w-full h-full" :option="authorChartOptions" autoresize />
                </div>
            </div>
        </div>

        <!-- Keyword Statistics -->
        <div v-if="activeTab === 'keywordStats'" class="card bg-base-100 shadow-xl">
            <div class="card-body">
                <h2 class="card-title">Keyword Trends by Year</h2>

                <div class="form-control mb-4">
                    <div class="flex gap-2">
                        <select class="select select-bordered">
                            <option>All Years</option>
                            <option>2023</option>
                            <option>2022</option>
                            <option>2021</option>
                            <option>2020</option>
                            <option>2019</option>
                        </select>
                        <input type="text" placeholder="Filter keywords" class="input input-bordered flex-1" />
                        <select class="select select-bordered">
                            <option>Sort by Frequency (High to Low)</option>
                            <option>Sort by Frequency (Low to High)</option>
                            <option>Sort by Keyword (A-Z)</option>
                            <option>Sort by Keyword (Z-A)</option>
                        </select>
                    </div>
                </div>

                <div class="overflow-x-auto">
                    <table class="table">
                        <thead>
                            <tr>
                                <th>Rank</th>
                                <th>Keyword</th>
                                <th>Frequency</th>
                                <th>Year-over-Year Change</th>
                                <th>Actions</th>
                            </tr>
                        </thead>
                        <tbody>
                            <tr>
                                <td>1</td>
                                <td>kw1</td>
                                <td>999</td>
                                <td class="text-success">+0%</td>
                                <td>
                                    <button class="btn btn-xs btn-ghost">View Papers</button>
                                </td>
                            </tr>

                        </tbody>
                    </table>
                </div>

                <!-- Mock visualization chart -->
                <div class="w-full h-80 bg-base-200 mt-6 flex items-center justify-center">
                    <div class="text-center">
                        <p class="text-lg mb-2">Keyword Trend Chart</p>
                        <p class="text-sm opacity-70">(Line chart visualization would be rendered here)</p>
                    </div>
                </div>
            </div>
        </div>
    </div>
</template>
