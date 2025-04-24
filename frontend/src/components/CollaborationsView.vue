<script setup>
import { ref, onMounted, onUnmounted, watch, nextTick, computed } from 'vue';
import api from '@/api';
import { useRouter, useRoute } from 'vue-router';
import VChart from 'vue-echarts';
import { GraphChart, BarChart } from 'echarts/charts';
import { GridComponent, TooltipComponent, DataZoomComponent, LegendComponent, TitleComponent } from 'echarts/components';
import { CanvasRenderer } from 'echarts/renderers';
import { use } from 'echarts/core';
import { getDaisyUIColor } from '@/utils/colors';
const router = useRouter();
const route = useRoute();

use([
    BarChart,
    CanvasRenderer,
    GraphChart,
    GridComponent,
    TooltipComponent,
    DataZoomComponent,
    LegendComponent,
    TitleComponent
]);


const activeTab = ref('networkGraph');
const authorName = ref('');
const collaborators = ref(null);
const selectedLink = ref(null);
const selectedAuthor = ref(null);
const coauthoredArticles = ref([]);
const collaborationCliques = ref(null);
const statsContainerRef = ref(null);
const isLoadingCollabNet = ref(false);
const isLoadingCollabArticles = ref(false);
const CliqueStep = ref(1); // 1: Initialize, 2: Build Adjacency, 3: Pivoter, 4: Complete
const CliqueProgress = ref(0.0);
const isRunningClique = ref(false);
let eventSource = null;

onMounted(() => {
    if (route.query.tab) {
        activeTab.value = route.query.tab;
    }

    if (route.query.authorName) {
        authorName.value = route.query.authorName;
        selectedLink.value = {
            author: route.query.authorName,
            coauthor: route.query.coauthor,
        }
        loadCollaborationNetwork();
        fetchCoauthoredArticles(route.query.authorName, route.query.coauthor)
    }
});

watch(() => route.query.author, (newAuthor) => {
    if (newAuthor) {
        authorName.value = newAuthor;
        loadCollaborationNetwork();
    }
});

watch(collaborationCliques, (newVal) => {
    if (newVal && newVal.length) {
        nextTick(() => {
            if (statsContainerRef.value) {
                statsContainerRef.value.scrollLeft = statsContainerRef.value.scrollWidth;
            }
        });
    }
}, { immediate: true });

const cliqueChartOptions = computed(() => {
    if (!collaborationCliques.value) return { series: [] };

    const cliques = collaborationCliques.value;

    return {
        title: {
            text: `Count of Collaboration Cliques of Different Sizes`,
            left: 'center',
            textStyle: {
                color: getDaisyUIColor("nc")
            },
        },
        grid: {
            left: '3%',
            right: '4%',
            bottom: '3%',
            containLabel: true
        },
        xAxis: {
            type: 'category',
            data: cliques.map(a => a.order),
            axisLabel: {
                width: 120,
                overflow: 'truncate'
            }
        },
        yAxis: {
            type: 'value',
            boundaryGap: [0, 0.01],
            axisLabel: {
                formatter: function (value) {
                    if (Math.abs(value) >= 1e5) {
                        return value.toExponential(2);
                    }
                    return value.toFixed(2);
                }
            }
        },
        dataZoom: [
            {
                type: 'slider',
                xAxisIndex: 0,
                start: 0,
                end: collaborationCliques.value.length
            },
            {
                type: 'inside',
                xAxisIndex: 0,
                zoomOnMouseWheel: true,
            }
        ],
        series: [
            {
                name: 'Cliques',
                type: 'bar',
                data: cliques.map(a => a.count),
                itemStyle: {
                    color: getDaisyUIColor('p')
                }
            }
        ],
        tooltip: {
            trigger: 'axis',
            axisPointer: {
                type: 'shadow'
            },
            backgroundColor: getDaisyUIColor('n'),
            borderColor: getDaisyUIColor('n'),
            textStyle: {
                color: getDaisyUIColor('nc')
            }
        },

    };
});

async function loadCollaborationNetwork() {
    if (!authorName.value) return;

    isLoadingCollabNet.value = true;
    try {
        const result = await api.author.getAuthorCollaborators(authorName.value);
        selectedAuthor.value = authorName.value;
        collaborators.value = result.data;
    } catch (error) {
        console.error('Error fetching author collaborators:', error);
    } finally {
        isLoadingCollabNet.value = false;
    }
}

const collaborationNetworkChartOptions = computed(() => {
    if (!collaborators.value) return {};

    // Prepare nodes and links for the graph
    const nodes = [{
        id: selectedAuthor.value,
        name: selectedAuthor.value, symbolSize: 30, category: 0,
        itemStyle: {
            color: getDaisyUIColor("p")
        }
    }];
    const links = [];

    Object.entries(collaborators.value).forEach(([coauthor, count]) => {
        // Add collaborator node
        nodes.push({
            id: coauthor,
            name: coauthor,
            symbolSize: 10 + Math.min(count * 2, 20), // Size based on collaboration count
            category: 1,
            itemStyle: {
                color: getDaisyUIColor("a")
            }
        });

        // Add link between main author and collaborator
        links.push({
            source: selectedAuthor.value,
            target: coauthor,
            value: count,
            lineStyle: {
                width: 1 + Math.min(count, 5),// Width based on collaboration count
                color: getDaisyUIColor("nc")
            },
        });
    });

    return {
        title: {
            text: `Collaboration Network for ${selectedAuthor.value}`,
            left: 'center',
            textStyle: {
                color: getDaisyUIColor("nc")
            },
        },
        tooltip: {
            axisPointer: {
                type: 'shadow'
            },
            backgroundColor: getDaisyUIColor('n'),
            borderColor: getDaisyUIColor('n'),
            textStyle: {
                color: getDaisyUIColor('nc')
            },
            formatter: function (params) {
                if (params.dataType === 'edge') {
                    return `${params.data.source} and ${params.data.target}: ${params.data.value} collaborations`;
                } else {
                    return params.data.name;
                }
            }
        },
        legend: [
            {
                data: ['Main Author', 'Collaborator'],
                bottom: 0,
                textStyle: {
                    color: getDaisyUIColor("nc")
                },
            }
        ],
        series: [{
            name: 'Collaboration Network',
            type: 'graph',
            layout: 'force',
            data: nodes,
            links: links,
            categories: [
                {
                    name: 'Main Author',
                    itemStyle: {
                        color: getDaisyUIColor("p")
                    }
                },
                {
                    name: 'Collaborator',
                    itemStyle: {
                        color: getDaisyUIColor("a")
                    }
                }
            ],
            roam: true,
            label: {
                show: true,
                position: 'right'
            },
            force: {
                repulsion: 100,
                edgeLength: 100
            },
            emphasis: {
                focus: 'adjacency',
                lineStyle: {
                    width: 4
                }
            }
        }]
    };
})

function handleCollaborationNetworkChartClick(params) {
    if (params.dataType === 'edge') {
        const source = params.data.source;
        const target = params.data.target;

        let author, coauthor;
        if (source === selectedAuthor.value) {
            author = source;
            coauthor = target;
        } else {
            author = target;
            coauthor = source;
        }

        selectedLink.value = { author, coauthor };
        fetchCoauthoredArticles(author, coauthor);
    } else if (params.dataType === 'node') {
        if (params.data.id !== selectedAuthor.value) {
            router.push({
                name: 'Collaborations',
                query: {
                    tab: 'networkGraph',
                    author: params.data.id
                }
            });
        }
    }
}

async function fetchCoauthoredArticles(author, coauthor) {
    isLoadingCollabArticles.value = true;
    try {
        const result = await api.author.getAuthorCoauthoredArticles(author, coauthor);
        coauthoredArticles.value = result.data;
    } catch (error) {
        console.error('Error fetching coauthored articles:', error);
        coauthoredArticles.value = [];
    } finally {
        isLoadingCollabArticles.value = false;
    }
}
function handleSearchClick() {
    if (!authorName.value.trim()) {
        alert('Please enter an author name');
        return;
    }
    loadCollaborationNetwork();
}
function viewArticle(articleId) {
    let queryParams = {
        returnPage: "Collaborations",
        returnTab: activeTab.value
    };

    if (activeTab.value === 'networkGraph' && authorName.value) {
        queryParams.authorName = authorName.value;
        queryParams.coauthor = selectedLink.value.coauthor;
    }

    router.push({
        name: 'ArticleDetail',
        params: { id: articleId },
        query: queryParams
    });
}

async function loadCollaborationCliques() {
    isRunningClique.value = true;
    CliqueStep.value = 1;
    CliqueProgress.value = 0;
    collaborationCliques.value = null;

    if (eventSource) {
        eventSource.close();
    }

    eventSource = new EventSource(api.config.apiRoot + '/stats/collaboration/complete-subgraphs');

    eventSource.onmessage = (event) => {
        const data = JSON.parse(event.data);

        if (data.status === 'build_adjacency_list') {
            if (data.progress > 98) {
                CliqueStep.value = 3;
            } else {
                CliqueStep.value = 2;
                CliqueProgress.value = data.progress;
            }
        }
        else if (data.status === 'pivoter') {
            CliqueStep.value = 3;
        }
        else if (data.status === 'done') {
            CliqueStep.value = 4;
            collaborationCliques.value = data.data;
            isRunningClique.value = false;
            eventSource.close();
        }
    };

    eventSource.onerror = (error) => {
        console.error('SSE Error:', error);
        isRunningClique.value = false;
        if (eventSource) {
            eventSource.close();
        }
    }
}

function formatLargeNumber(value) {
    if (!value) return '0';

    const threshold = 10000;

    if (Math.abs(value) < threshold) {
        return value.toString();
    } else {
        const exp = Math.floor(Math.log10(Math.abs(value)));
        const mantissa = value / Math.pow(10, exp);
        const formattedMantissa = mantissa.toFixed(2);
        const trimmedMantissa = formattedMantissa.replace(/\.?0+$/, '');

        const superscript = exp.toString().split('').map(digit => {
            const superscriptMap = {
                '0': '⁰', '1': '¹', '2': '²', '3': '³', '4': '⁴',
                '5': '⁵', '6': '⁶', '7': '⁷', '8': '⁸', '9': '⁹',
                '-': '⁻'
            };
            return superscriptMap[digit] || digit;
        }).join('');
        return `${trimmedMantissa} × 10${superscript}`;
    }
}
watch(activeTab, (newValue, oldValue) => {
    if (newValue === 'networkGraph') {
    } else if (newValue === 'cliques') {
        loadCollaborationCliques();
    }
}, { immediate: true });

</script>

<template>
    <div>
        <h2 class="text-2xl font-bold mb-6">Author Collaborations</h2>
        <div class="tabs tabs-boxed mb-6">
            <a class="tab" :class="{ 'tab-active': activeTab === 'networkGraph' }"
                @click="activeTab = 'networkGraph'">Collaboration Network</a>
            <a class="tab" :class="{ 'tab-active': activeTab === 'cliques' }"
                @click="activeTab = 'cliques'">Collaboration Cliques</a>
        </div>
        <!-- Collaboration Network -->
        <div v-if="activeTab === 'networkGraph'" class="card bg-base-100 shadow-xl">
            <div class="card-body">
                <h2 class="card-title">Author Collaboration Network</h2>
                <div class="form-control mb-4">
                    <div class="flex gap-2">
                        <input type="text" v-model="authorName" placeholder="Enter author name"
                            class="input input-bordered flex-1" />
                        <button class="btn btn-primary" @click="handleSearchClick" :disabled="isLoadingCollabNet">
                            <span v-if="isLoadingCollabNet">Loading...</span>
                            <span v-else>Search</span>
                        </button>
                    </div>
                </div>
                <!-- Loading indicator -->
                <div v-if="isLoadingCollabNet" class="flex justify-center my-4">
                    <span class="loading loading-spinner loading-lg"></span>
                </div>
                <!-- Network visualization -->
                <div v-else-if="collaborators" class="w-full h-96">
                    <v-chart class="w-full h-full" :option="collaborationNetworkChartOptions"
                        @click="handleCollaborationNetworkChartClick" autoresize />
                </div>

                <!-- Coauthored articles table -->
                <div v-if="isLoadingCollabArticles" class="flex justify-center my-4">
                    <span class="loading loading-spinner loading-lg"></span>
                </div>
                <div v-else-if="selectedLink && coauthoredArticles.length > 0" class="mt-6">
                    <h3 class="font-bold text-lg mb-4">
                        Collaborations between {{ selectedLink.author }} and {{ selectedLink.coauthor }}
                    </h3>

                    <div class="overflow-x-auto">
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
                                <tr v-for="article in coauthoredArticles" :key="article.article_id">
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

                <div v-else-if="selectedLink" class="mt-6 text-center py-4 text-gray-500">
                    No collaboration articles found between selected authors
                </div>
            </div>
        </div>
        <!-- Collaboration Cliques -->
        <div v-if="activeTab === 'cliques'" class="card bg-base-100 shadow-xl">
            <div class="card-body">
                <h2 class="card-title">Collaboration Cliques</h2>

                <p class="mb-4">
                    This view shows fully connected subgraphs (cliques) of research collaborations.
                    A k-clique represents a group where every author has collaborated with every other author in the
                    group.
                </p>

                <!-- Progress Steps -->
                <ul class="steps w-full mb-6">
                    <li class="step" :class="{ 'step-primary': CliqueStep >= 1 }">
                        Initialize
                    </li>
                    <li class="step" :class="{ 'step-primary': CliqueStep >= 2 }">
                        Build Adjacency List
                    </li>
                    <li class="step" :class="{ 'step-primary': CliqueStep >= 3 }">
                        Run Pivoter Algorithm
                    </li>
                    <li class="step" :class="{ 'step-primary': CliqueStep >= 4 }">
                        Complete
                    </li>
                </ul>

                <!-- Progress Indicators -->
                <div class="grid gap-4">
                    <!-- Building Adjacency List Progress -->
                    <div v-if="CliqueStep === 2" class="w-full">
                        <div class="flex justify-between mb-1">
                            <span class="text-sm font-medium">Building Adjacency List</span>
                            <span class="text-sm font-medium">{{ Math.round(CliqueProgress) }}%</span>
                        </div>
                        <progress class="progress progress-primary w-full" :value="CliqueProgress" max="100"></progress>
                    </div>

                    <!-- Progress -->
                    <div v-if="CliqueStep === 3 || CliqueStep === 1" class="flex items-center justify-center gap-3">
                        <span class="text-sm font-medium">{{
                            [null, "Initializing", null, "Running Pivoter Algorithm"].at(CliqueStep)
                            }}</span>
                        <span class="loading loading-spinner text-primary"></span>
                    </div>
                </div>

                <div v-if="!isRunningClique && collaborationCliques">
                    <div class="overflow-x-auto pb-2" ref="statsContainerRef">
                        <div class="stats shadow mb-6 flex flex-nowrap min-w-max">
                            <div v-for="cliqueinfo in collaborationCliques" class="stat">
                                <div class="stat-title">{{ cliqueinfo.order }}-Cliques</div>
                                <div class="stat-value" v-html="formatLargeNumber(cliqueinfo.count)"></div>
                            </div>
                        </div>
                    </div>

                    <div class="w-full h-80">
                        <v-chart class="w-full h-full" :option="cliqueChartOptions" autoresize />
                    </div>
                </div>

            </div>
        </div>
    </div>
</template>
