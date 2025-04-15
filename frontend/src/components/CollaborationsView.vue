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
                        <input type="text" placeholder="Filter by author" class="input input-bordered flex-1" />
                        <select class="select select-bordered">
                            <option>All Years</option>
                            <option>Last 5 Years</option>
                            <option>Last 10 Years</option>
                            <option>Custom Range</option>
                        </select>
                        <button class="btn btn-primary">Apply</button>
                    </div>
                </div>

                <!-- Network visualization placeholder -->
                <div class="w-full h-96 bg-base-200 flex items-center justify-center">
                    <div class="text-center">
                        <p class="text-lg mb-2">Collaboration Network Visualization</p>
                        <p class="text-sm opacity-70">(Interactive network graph would be rendered here)</p>
                        <p class="text-xs opacity-50 mt-2">Nodes represent authors and edges represent collaborations
                        </p>
                    </div>
                </div>

                <div class="card bg-base-300 mt-4">
                    <div class="card-body">
                        <h3 class="card-title text-lg">Network Statistics</h3>
                        <div class="stats stats-vertical lg:stats-horizontal shadow w-full">
                            <div class="stat">
                                <div class="stat-title">Authors</div>
                                <div class="stat-value">234</div>
                                <div class="stat-desc">Total nodes in network</div>
                            </div>

                            <div class="stat">
                                <div class="stat-title">Collaborations</div>
                                <div class="stat-value">765</div>
                                <div class="stat-desc">Total edges in network</div>
                            </div>

                            <div class="stat">
                                <div class="stat-title">Average Collaborators</div>
                                <div class="stat-value">6.5</div>
                                <div class="stat-desc">Per author</div>
                            </div>

                            <div class="stat">
                                <div class="stat-title">Largest Community</div>
                                <div class="stat-value">42</div>
                                <div class="stat-desc">Authors in largest cluster</div>
                            </div>
                        </div>
                    </div>
                </div>

                <!-- Selected author details would appear here -->
                <div class="card bg-base-200 mt-4">
                    <div class="card-body">
                        <h3 class="card-title text-lg">Selected Author: Author A</h3>
                        <div class="grid grid-cols-1 md:grid-cols-2 gap-4">
                            <div>
                                <h4 class="font-bold">Collaborator Network</h4>
                                <ul class="list-disc pl-5 mt-2">
                                    <li>Author B (12 joint papers)</li>
                                    <li>Author C (8 joint papers)</li>
                                    <li>Author D (5 joint papers)</li>
                                    <li>Author E (3 joint papers)</li>
                                    <li>Author F (2 joint papers)</li>
                                </ul>
                            </div>
                            <div>
                                <h4 class="font-bold">Recent Joint Publications</h4>
                                <ul class="list-disc pl-5 mt-2">
                                    <li>Paper Title 1 (2023) with Authors B, C</li>
                                    <li>Paper Title 2 (2022) with Author B</li>
                                    <li>Paper Title 3 (2022) with Authors D, E</li>
                                    <li>Paper Title 4 (2021) with Author C</li>
                                    <li>Paper Title 5 (2021) with Authors F, D</li>
                                </ul>
                            </div>
                        </div>
                    </div>
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

<script setup>
import { ref } from 'vue';

// Active tab state
const activeTab = ref('networkGraph');
</script>