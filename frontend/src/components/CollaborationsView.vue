<script setup>
import { ref, onMounted, onUnmounted, watch } from 'vue';
import api from '@/api';
import { useRouter, useRoute } from 'vue-router';
import * as echarts from 'echarts';
const router = useRouter();
const route = useRoute();

const activeTab = ref('networkGraph');
const authorName = ref('');
const collaboratorChart = ref(null);
const collaborators = ref({});
const selectedLink = ref(null);
const selectedAuthor = ref(null);
const coauthoredArticles = ref([]);
const isLoading = ref(false);

onMounted(() => {
    const chartContainer = document.getElementById('collaborationNetwork');
    if (chartContainer) {
        collaboratorChart.value = echarts.init(chartContainer);

        window.addEventListener('resize', () => {
            collaboratorChart.value.resize();
        });
    }

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
onUnmounted(() => {
    if (collaboratorChart.value) {
        window.removeEventListener('resize', collaboratorChart.value.resize())
        collaboratorChart.value.dispose();
    }
});

async function loadCollaborationNetwork() {
    console.log('Loading collaboration network for author:', authorName.value);
    if (!authorName.value) return;

    isLoading.value = true;
    try {
        const result = await api.author.getAuthorCollaborators(authorName.value);
        collaborators.value = result.data;
        selectedAuthor.value = authorName.value;
        renderCollaborationNetwork();
    } catch (error) {
        console.error('Error fetching author collaborators:', error);
    } finally {
        isLoading.value = false;
    }
}
function renderCollaborationNetwork() {
    if (!collaboratorChart.value || !collaborators.value) return;

    // Prepare nodes and links for the graph
    const nodes = [{ id: selectedAuthor.value, name: selectedAuthor.value, symbolSize: 30, category: 0 }];
    const links = [];

    Object.entries(collaborators.value).forEach(([coauthor, count]) => {
        // Add collaborator node
        nodes.push({
            id: coauthor,
            name: coauthor,
            symbolSize: 10 + Math.min(count * 2, 20), // Size based on collaboration count
            category: 1
        });

        // Add link between main author and collaborator
        links.push({
            source: selectedAuthor.value,
            target: coauthor,
            value: count,
            lineStyle: {
                width: 1 + Math.min(count, 5) // Width based on collaboration count
            }
        });
    });

    const option = {
        title: {
            text: `Collaboration Network for ${selectedAuthor.value}`,
            top: 'top',
            left: 'center'
        },
        tooltip: {
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
                bottom: 0
            }
        ],
        series: [{
            name: 'Collaboration Network',
            type: 'graph',
            layout: 'force',
            data: nodes,
            links: links,
            categories: [
                { name: 'Main Author' },
                { name: 'Collaborator' }
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

    collaboratorChart.value.setOption(option);

    collaboratorChart.value.on('click', function (params) {
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
                console.log('Navigating to author:', params.data.id);
            }
        }
    });
}

async function fetchCoauthoredArticles(author, coauthor) {
    isLoading.value = true;
    try {
        const result = await api.author.getAuthorCoauthoredArticles(author, coauthor);
        coauthoredArticles.value = result.data;
    } catch (error) {
        console.error('Error fetching coauthored articles:', error);
        coauthoredArticles.value = [];
    } finally {
        isLoading.value = false;
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

    console.log(queryParams)

    router.push({
        name: 'ArticleDetail',
        params: { id: articleId },
        query: queryParams
    });
}
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
                        <button class="btn btn-primary" @click="handleSearchClick" :disabled="isLoading">
                            <span v-if="isLoading">Loading...</span>
                            <span v-else>Search</span>
                        </button>
                    </div>
                </div>
                <!-- Network visualization -->
                <div id="collaborationNetwork" class="w-full h-96 bg-base-200"></div>
                <!-- Loading indicator -->
                <div v-if="isLoading" class="flex justify-center my-4">
                    <span class="loading loading-spinner loading-lg"></span>
                </div>
                <!-- Coauthored articles table -->
                <div v-if="selectedLink && coauthoredArticles.length > 0" class="mt-6">
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

                <div class="stats shadow mb-6">
                    <div class="stat">
                        <div class="stat-title">2-Cliques</div>
                        <div class="stat-value">467</div>
                        <div class="stat-desc">Simple collaborations</div>
                    </div>

                    <div class="stat">
                        <div class="stat-title">3-Cliques</div>
                        <div class="stat-value">178</div>
                        <div class="stat-desc">Triangles</div>
                    </div>

                    <div class="stat">
                        <div class="stat-title">4-Cliques</div>
                        <div class="stat-value">53</div>
                        <div class="stat-desc">Tetrahedrons</div>
                    </div>

                    <div class="stat">
                        <div class="stat-title">5-Cliques</div>
                        <div class="stat-value">14</div>
                    </div>

                    <div class="stat">
                        <div class="stat-title">6-Cliques</div>
                        <div class="stat-value">3</div>
                    </div>
                </div>

                <!-- Clique visualization placeholder -->
                <div class="w-full h-80 bg-base-200 flex items-center justify-center">
                    <div class="text-center">
                        <p class="text-lg mb-2">Clique Distribution Visualization</p>
                        <p class="text-sm opacity-70">(Bar chart visualization would be rendered here)</p>
                    </div>
                </div>

                <div class="divider">Largest Cliques</div>

                <div class="overflow-x-auto">
                    <table class="table">
                        <thead>
                            <tr>
                                <th>Clique Size</th>
                                <th>Authors</th>
                                <th>Joint Papers</th>
                                <th>Years Active</th>
                                <th>Actions</th>
                            </tr>
                        </thead>
                        <tbody>
                            <tr>
                                <td>6</td>
                                <td>Authors A, B, C, D, E, F</td>
                                <td>12</td>
                                <td>2018-2023</td>
                                <td>
                                    <button class="btn btn-xs btn-ghost">View Details</button>
                                </td>
                            </tr>
                            <tr>
                                <td>6</td>
                                <td>Authors G, H, I, J, K, L</td>
                                <td>8</td>
                                <td>2019-2022</td>
                                <td>
                                    <button class="btn btn-xs btn-ghost">View Details</button>
                                </td>
                            </tr>
                            <tr>
                                <td>6</td>
                                <td>Authors M, N, O, P, Q, R</td>
                                <td>7</td>
                                <td>2017-2023</td>
                                <td>
                                    <button class="btn btn-xs btn-ghost">View Details</button>
                                </td>
                            </tr>
                            <tr>
                                <td>5</td>
                                <td>Authors S, T, U, V, W</td>
                                <td>15</td>
                                <td>2016-2023</td>
                                <td>
                                    <button class="btn btn-xs btn-ghost">View Details</button>
                                </td>
                            </tr>
                            <tr>
                                <td>5</td>
                                <td>Authors X, Y, Z, A1, B1</td>
                                <td>11</td>
                                <td>2020-2023</td>
                                <td>
                                    <button class="btn btn-xs btn-ghost">View Details</button>
                                </td>
                            </tr>
                        </tbody>
                    </table>
                </div>
            </div>
        </div>
    </div>
</template>
