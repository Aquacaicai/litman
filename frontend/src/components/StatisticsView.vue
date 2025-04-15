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

                <div class="form-control mb-4">
                    <div class="flex gap-2">
                        <input type="text" placeholder="Filter authors" class="input input-bordered flex-1" />
                        <select class="select select-bordered">
                            <option>Sort by Publication Count (High to Low)</option>
                            <option>Sort by Publication Count (Low to High)</option>
                            <option>Sort by Author Name (A-Z)</option>
                            <option>Sort by Author Name (Z-A)</option>
                        </select>
                    </div>
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
                            <tr>
                                <td>1</td>
                                <td>Author, A.</td>
                                <td>42</td>
                                <td>
                                    <button class="btn btn-xs btn-ghost">View Papers</button>
                                </td>
                            </tr>
                            <tr>
                                <td>2</td>
                                <td>Author, B.</td>
                                <td>38</td>
                                <td>
                                    <button class="btn btn-xs btn-ghost">View Papers</button>
                                </td>
                            </tr>
                            <tr>
                                <td>3</td>
                                <td>Author, C.</td>
                                <td>35</td>
                                <td>
                                    <button class="btn btn-xs btn-ghost">View Papers</button>
                                </td>
                            </tr>
                            <tr>
                                <td>4</td>
                                <td>Author, D.</td>
                                <td>27</td>
                                <td>
                                    <button class="btn btn-xs btn-ghost">View Papers</button>
                                </td>
                            </tr>
                            <tr>
                                <td>5</td>
                                <td>Author, E.</td>
                                <td>23</td>
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
                        <p class="text-lg mb-2">Author Publication Count Chart</p>
                        <p class="text-sm opacity-70">(Bar chart visualization would be rendered here)</p>
                    </div>
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

<script setup>
import { ref } from 'vue';

// Active tab state
const activeTab = ref('authorStats');
</script>