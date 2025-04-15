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
                        <input type="text" placeholder="Enter author name" class="input input-bordered flex-1" />
                        <button class="btn btn-primary">Search</button>
                    </div>
                </div>

                <div class="divider">Results</div>

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
                            <tr>
                                <td>111</td>
                                <td>111</td>
                                <td>111</td>
                                <td>
                                    <button class="btn btn-xs btn-ghost">View</button>
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
                        <input type="text" placeholder="Enter paper title" class="input input-bordered flex-1" />
                        <button class="btn btn-primary">Search</button>
                    </div>
                </div>

                <div class="divider">Results</div>

                <div class="card bg-base-200">
                    <div class="card-body">
                        <h2 class="card-title">Example Paper Title</h2>
                        <p>Year: 2023</p>
                        <p>Authors: Author, A., Author, B., Author, C.</p>
                        <p>Keywords: keyword1, keyword2, keyword3</p>
                        <p>Abstract: This is a sample abstract of the paper...</p>
                    </div>
                </div>
            </div>
        </div>

        <!-- Search by Collaborator 
        <div v-if="activeTab === 'byCollaborator'" class="card bg-base-100 shadow-xl">
            <div class="card-body">
                <h2 class="card-title">Search by Collaborator</h2>
                <div class="form-control">
                    <label class="label">
                        <span class="label-text">Author Name</span>
                    </label>
                    <div class="flex gap-2">
                        <input type="text" placeholder="Enter author name" class="input input-bordered flex-1" />
                        <button class="btn btn-primary">Find Collaborators</button>
                    </div>
                </div>

                <div class="divider">Collaborators</div>

                <div class="overflow-x-auto">
                    <table class="table">
                        <thead>
                            <tr>
                                <th>Collaborator</th>
                                <th>Joint Papers</th>
                                <th>Actions</th>
                            </tr>
                        </thead>
                        <tbody>
                            <tr>
                                <td>Author, B.</td>
                                <td>3</td>
                                <td>
                                    <button class="btn btn-xs btn-ghost">View Papers</button>
                                </td>
                            </tr>
                            <tr>
                                <td>Author, C.</td>
                                <td>2</td>
                                <td>
                                    <button class="btn btn-xs btn-ghost">View Papers</button>
                                </td>
                            </tr>
                        </tbody>
                    </table>
                </div>
            </div>
        </div>-->

        <!-- Fuzzy Search -->
        <div v-if="activeTab === 'fuzzySearch'" class="card bg-base-100 shadow-xl">
            <div class="card-body">
                <h2 class="card-title">Fuzzy Title Search</h2>
                <div class="form-control">
                    <label class="label">
                        <span class="label-text">Title Keywords</span>
                    </label>
                    <div class="flex gap-2">
                        <input type="text" placeholder="Enter title keywords" class="input input-bordered flex-1" />
                        <button class="btn btn-primary">Search</button>
                    </div>
                </div>

                <div class="divider">Results</div>

                <div class="overflow-x-auto">
                    <table class="table">
                        <thead>
                            <tr>
                                <th>Title</th>
                                <th>Year</th>
                                <th>Authors</th>
                                <th>Match Score</th>
                                <th>Actions</th>
                            </tr>
                        </thead>
                        <tbody>
                            <tr>
                                <td>111</td>
                                <td>111</td>
                                <td>111</td>
                                <td>111</td>
                                <td>
                                    <button class="btn btn-xs btn-ghost">View</button>
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

const activeTab = ref('byAuthor');
</script>