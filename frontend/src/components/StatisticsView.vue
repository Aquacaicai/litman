<script setup>
import { ref, watch, computed } from 'vue';
import { ChevronDoubleLeftIcon, ChevronDoubleRightIcon } from '@heroicons/vue/16/solid'
import api from '@/api';
import { useRouter } from 'vue-router';
import { use } from 'echarts/core';
import { CanvasRenderer } from 'echarts/renderers';
import { BarChart, LineChart } from 'echarts/charts';
import { GridComponent, TooltipComponent, DataZoomComponent } from 'echarts/components';
import VChart from 'vue-echarts';

// Register ECharts components
use([
    CanvasRenderer,
    BarChart,
    LineChart,
    GridComponent,
    TooltipComponent,
    DataZoomComponent
]);

const router = useRouter();
const activeTab = ref('authorStats');
const authorFilter = ref('');

const authorStats = ref(undefined);
const keywordsStats = ref(undefined);
const selectedYear = ref(null);
const currentPage = ref(1);
const itemsPerPage = 10;
const isLoading = ref(false);

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
    isLoading.value = true;

    try {
        const results = await api.stats.getAuthorArticleCounts();
        authorStats.value = results.data;
    } catch (error) {
        console.error('Error fetching author\'s article count:', error);
    } finally {
        isLoading.value = false;
    }
}

async function loadKeywordTrends() {
    isLoading.value = true;

    try {
        const results = await api.stats.getYearlyKeywordFrequencies()
        keywordsStats.value = results.data;

        // Set selected year to the most recent year by default
        if (keywordsStats.value && Object.keys(keywordsStats.value).length > 0) {
            const years = Object.keys(keywordsStats.value).map(Number).sort((a, b) => b - a);
            selectedYear.value = years[0].toString();

            // Scroll to the latest year button after a small delay to ensure rendering
            setTimeout(() => {
                const yearButtons = document.querySelectorAll('.join-item.btn');
                if (yearButtons && yearButtons.length > 0) {
                    const lastButton = yearButtons[yearButtons.length - 1];
                    lastButton.scrollIntoView({ behavior: 'smooth', block: 'nearest', inline: 'end' });
                }
            }, 100);
        }
    } catch (error) {
        console.error('Error fetching keyword freq:', error);
    } finally {
        isLoading.value = false;
    }
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

// Compute available years from keywordsStats
const availableYears = computed(() => {
    if (!keywordsStats.value) return [];
    // Sort years in ascending order (older to newer)
    return Object.keys(keywordsStats.value).map(Number).sort((a, b) => a - b);
});

// Compute current year's top keywords
const currentYearKeywords = computed(() => {
    if (!keywordsStats.value || !selectedYear.value) return [];

    const yearData = keywordsStats.value[selectedYear.value] || [];
    return [...yearData].sort((a, b) => b.frequency - a.frequency);
});

// Compute keyword trend data for chart
const keywordTrendChartOptions = computed(() => {
    if (!keywordsStats.value) return { series: [] };

    // Get top 5 keywords from current selected year
    const topKeywords = currentYearKeywords.value.slice(0, 5).map(k => k.keyword);

    // Get all years sorted
    const years = availableYears.value.map(String);

    // Create series data for each top keyword
    const series = topKeywords.map(keyword => {
        const data = years.map(year => {
            const yearData = keywordsStats.value[year] || [];
            const keywordData = yearData.find(k => k.keyword === keyword);
            return keywordData ? keywordData.frequency : 0;
        });

        return {
            name: keyword,
            type: 'line',
            data: data,
            smooth: true
        };
    });

    return {
        tooltip: {
            trigger: 'axis',
        },
        legend: {
            data: topKeywords,
            orient: 'horizontal',
            bottom: 0
        },
        grid: {
            left: '3%',
            right: '4%',
            bottom: '10%',
            containLabel: true
        },
        xAxis: {
            type: 'category',
            boundaryGap: false,
            data: years
        },
        yAxis: {
            type: 'value',
            name: 'Frequency'
        },
        series: series
    };
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


                <div v-if="isLoading" class="flex justify-center my-4">
                    <span class="loading loading-spinner loading-lg"></span>
                </div>

                <div v-else-if="authorStats">
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

                <div v-else class="w-full h-80 bg-base-200 mt-6 flex items-center justify-center">
                    <div class="text-center">
                        <p class="text-lg mb-2">No keyword data available</p>
                    </div>
                </div>
            </div>
        </div>

        <!-- Keyword Statistics -->
        <div v-if="activeTab === 'keywordStats'" class="card bg-base-100 shadow-xl">
            <div class="card-body">
                <h2 class="card-title">Keyword Trends by Year</h2>

                <div v-if="isLoading" class="flex justify-center my-4">
                    <span class="loading loading-spinner loading-lg"></span>
                </div>
                <div v-else-if="keywordsStats && selectedYear" class="overflow-x-auto">
                    <div class="form-control mb-4">
                        <div class="overflow-x-auto" style="max-width: 100%">
                            <div class="join whitespace-nowrap" style="padding-bottom: 5px;">
                                <button v-for="year in availableYears" :key="year" class="join-item btn"
                                    :class="{ 'btn-active': selectedYear === year.toString() }"
                                    @click="selectedYear = year.toString()">
                                    {{ year }}
                                </button>
                            </div>
                        </div>
                    </div>
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
                            <tr v-for="(keyword, index) in currentYearKeywords" :key="keyword.keyword">
                                <td>{{ index + 1 }}</td>
                                <td>{{ keyword.keyword }}</td>
                                <td>{{ keyword.frequency.toFixed(2) }}</td>
                                <td>
                                    <template v-if="Number(selectedYear) > Math.min(...availableYears)">
                                        <template v-if="keywordsStats[Number(selectedYear) - 1]">
                                            <template
                                                v-if="keywordsStats[Number(selectedYear) - 1].find(k => k.keyword === keyword.keyword)">
                                                <span :class="{
                                                    'text-success': keyword.frequency > (keywordsStats[Number(selectedYear) - 1].find(k => k.keyword === keyword.keyword)?.frequency || 0),
                                                    'text-error': keyword.frequency < (keywordsStats[Number(selectedYear) - 1].find(k => k.keyword === keyword.keyword)?.frequency || 0)
                                                }">
                                                    {{
                                                        ((keyword.frequency - (keywordsStats[Number(selectedYear) -
                                                            1].find(k => k.keyword === keyword.keyword)?.frequency || 0)) /
                                                            (keywordsStats[Number(selectedYear) - 1].find(k => k.keyword ===
                                                                keyword.keyword)?.frequency || 1) * 100).toFixed(1)
                                                    }}%
                                                </span>
                                            </template>
                                            <template v-else>
                                                <span class="text-success font-bold">NEW</span>
                                            </template>
                                        </template>
                                        <template v-else>
                                            <span>-</span>
                                        </template>
                                    </template>
                                    <template v-else>
                                        <span>-</span>
                                    </template>
                                </td>
                                <td>
                                    <button class="btn btn-xs btn-ghost" @click="router.push({
                                        name: 'LiteratureSearch', query: {
                                            tab: 'fuzzySearch',
                                            fuzzyTitle: keyword.keyword
                                        }
                                    })">View
                                        Papers</button>
                                </td>
                            </tr>
                        </tbody>
                    </table>
                </div>

                <!-- Keyword trend chart -->
                <div v-if="keywordsStats && Object.keys(keywordsStats).length > 0" class="w-full h-80 mt-6">
                    <h3 class="text-lg font-semibold mb-2">Keyword Trend Over Time</h3>
                    <v-chart class="w-full h-full" :option="keywordTrendChartOptions" autoresize />
                </div>
                <div v-else class="w-full h-80 bg-base-200 mt-6 flex items-center justify-center">
                    <div class="text-center">
                        <p class="text-lg mb-2">No keyword data available</p>
                    </div>
                </div>
            </div>
        </div>
    </div>
</template>
