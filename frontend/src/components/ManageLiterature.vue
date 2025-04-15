<template>
    <div>
        <h2 class="text-2xl font-bold mb-6">Manage Literature</h2>

        <div class="tabs tabs-boxed mb-6">
            <a class="tab" :class="{ 'tab-active': activeTab === 'import' }" @click="activeTab = 'import'">Import
                Literature</a>
            <a class="tab" :class="{ 'tab-active': activeTab === 'add' }" @click="activeTab = 'add'">Add Manually</a>
            <a class="tab" :class="{ 'tab-active': activeTab === 'manage' }" @click="activeTab = 'manage'">Manage
                Existing</a>
        </div>

        <!-- Import Literature -->
        <div v-if="activeTab === 'import'" class="card bg-base-100 shadow-xl">
            <div class="card-body">
                <h2 class="card-title">Import from XML</h2>

                <div class="form-control">
                    <label class="label">
                        <span class="label-text">Upload XML File</span>
                    </label>
                    <input type="file" class="file-input file-input-bordered w-full" />
                    <label class="label">
                        <span class="label-text-alt">Supported formats: XML files with literature metadata</span>
                    </label>
                </div>
                <div class="flex justify-end mt-4">
                    <button class="btn btn-primary">Import Literature</button>
                </div>

                <div class="divider">Import Options</div>

                <div class="form-control">
                    <div class="flex flex-wrap gap-4">
                        <label class="label cursor-pointer">
                            <span class="label-text mr-2">Skip duplicates</span>
                            <input type="checkbox" checked class="checkbox" />
                        </label>
                        <label class="label cursor-pointer">
                            <span class="label-text mr-2">Merge author information</span>
                            <input type="checkbox" checked class="checkbox" />
                        </label>
                        <label class="label cursor-pointer">
                            <span class="label-text mr-2">Update existing entries</span>
                            <input type="checkbox" class="checkbox" />
                        </label>
                    </div>
                </div>

                <!-- Import History -->
                <div class="mt-6">
                    <h3 class="font-bold mb-2">Recent Imports</h3>
                    <div class="overflow-x-auto">
                        <table class="table">
                            <thead>
                                <tr>
                                    <th>Date</th>
                                    <th>Filename</th>
                                    <th>Records</th>
                                    <th>Status</th>
                                </tr>
                            </thead>
                            <tbody>
                                <tr>
                                    <td>2023-10-15</td>
                                    <td>conference_papers.xml</td>
                                    <td>123</td>
                                    <td><span class="badge badge-success">Completed</span></td>
                                </tr>
                                <tr>
                                    <td>2023-10-10</td>
                                    <td>journal_articles.xml</td>
                                    <td>87</td>
                                    <td><span class="badge badge-success">Completed</span></td>
                                </tr>
                                <tr>
                                    <td>2023-09-28</td>
                                    <td>research_papers.xml</td>
                                    <td>42</td>
                                    <td><span class="badge badge-error">Failed</span></td>
                                </tr>
                            </tbody>
                        </table>
                    </div>
                </div>
            </div>
        </div>

        <!-- Add Manually -->
        <div v-if="activeTab === 'add'" class="card bg-base-100 shadow-xl">
            <div class="card-body">
                <h2 class="card-title">Add Literature Manually</h2>

                <div class="form-control">
                    <label class="label">
                        <span class="label-text">Paper Title *</span>
                    </label>
                    <input type="text" placeholder="Enter title" class="input input-bordered" />
                </div>

                <div class="form-control">
                    <label class="label">
                        <span class="label-text">Authors *</span>
                        <span class="label-text-alt">Format: Lastname, F.; Lastname, F.</span>
                    </label>
                    <input type="text" placeholder="Enter authors" class="input input-bordered" />
                </div>

                <div class="grid grid-cols-1 md:grid-cols-2 gap-4">
                    <div class="form-control">
                        <label class="label">
                            <span class="label-text">Year *</span>
                        </label>
                        <input type="number" placeholder="YYYY" class="input input-bordered" />
                    </div>

                    <div class="form-control">
                        <label class="label">
                            <span class="label-text">Publication Type</span>
                        </label>
                        <select class="select select-bordered w-full">
                            <option>Journal Article</option>
                            <option>Conference Paper</option>
                            <option>Book Chapter</option>
                            <option>Technical Report</option>
                            <option>Preprint</option>
                            <option>Other</option>
                        </select>
                    </div>
                </div>

                <div class="form-control">
                    <label class="label">
                        <span class="label-text">Journal/Conference</span>
                    </label>
                    <input type="text" placeholder="Enter publication venue" class="input input-bordered" />
                </div>

                <div class="form-control">
                    <label class="label">
                        <span class="label-text">Keywords</span>
                        <span class="label-text-alt">Comma separated</span>
                    </label>
                    <input type="text" placeholder="keyword1, keyword2, keyword3" class="input input-bordered" />
                </div>

                <div class="form-control">
                    <label class="label">
                        <span class="label-text">Abstract</span>
                    </label>
                    <textarea class="textarea textarea-bordered h-24" placeholder="Enter abstract"></textarea>
                </div>

                <div class="form-control">
                    <label class="label">
                        <span class="label-text">DOI</span>
                    </label>
                    <input type="text" placeholder="e.g., 10.1000/xyz123" class="input input-bordered" />
                </div>

                <div class="flex justify-end mt-4 gap-2">
                    <button class="btn btn-ghost">Reset</button>
                    <button class="btn btn-primary">Add Literature</button>
                </div>
            </div>
        </div>

        <!-- Manage Existing -->
        <div v-if="activeTab === 'manage'" class="card bg-base-100 shadow-xl">
            <div class="card-body">
                <h2 class="card-title">Manage Existing Literature</h2>

                <div class="form-control mb-4">
                    <div class="flex gap-2">
                        <input type="text" placeholder="Search literature" class="input input-bordered flex-1" />
                        <select class="select select-bordered">
                            <option>All Types</option>
                            <option>Journal Articles</option>
                            <option>Conference Papers</option>
                            <option>Book Chapters</option>
                            <option>Technical Reports</option>
                        </select>
                        <button class="btn btn-primary">Search</button>
                    </div>
                </div>

                <div class="overflow-x-auto">
                    <table class="table">
                        <thead>
                            <tr>
                                <th>Title</th>
                                <th>Authors</th>
                                <th>Year</th>
                                <th>Type</th>
                                <th>Actions</th>
                            </tr>
                        </thead>
                        <tbody>
                            <tr>
                                <td>Sample Paper Title 1</td>
                                <td>Author, A., Author, B.</td>
                                <td>2023</td>
                                <td>Journal Article</td>
                                <td>
                                    <div class="flex gap-1">
                                        <button class="btn btn-xs btn-ghost">Edit</button>
                                        <button class="btn btn-xs btn-error">Delete</button>
                                    </div>
                                </td>
                            </tr>
                            <tr>
                                <td>Sample Paper Title 2</td>
                                <td>Author, C., Author, D.</td>
                                <td>2022</td>
                                <td>Conference Paper</td>
                                <td>
                                    <div class="flex gap-1">
                                        <button class="btn btn-xs btn-ghost">Edit</button>
                                        <button class="btn btn-xs btn-error">Delete</button>
                                    </div>
                                </td>
                            </tr>
                            <tr>
                                <td>Sample Paper Title 3</td>
                                <td>Author, A., Author, E.</td>
                                <td>2022</td>
                                <td>Journal Article</td>
                                <td>
                                    <div class="flex gap-1">
                                        <button class="btn btn-xs btn-ghost">Edit</button>
                                        <button class="btn btn-xs btn-error">Delete</button>
                                    </div>
                                </td>
                            </tr>
                            <tr>
                                <td>Sample Paper Title 4</td>
                                <td>Author, F., Author, G., Author, H.</td>
                                <td>2021</td>
                                <td>Book Chapter</td>
                                <td>
                                    <div class="flex gap-1">
                                        <button class="btn btn-xs btn-ghost">Edit</button>
                                        <button class="btn btn-xs btn-error">Delete</button>
                                    </div>
                                </td>
                            </tr>
                            <tr>
                                <td>Sample Paper Title 5</td>
                                <td>Author, B., Author, I.</td>
                                <td>2021</td>
                                <td>Technical Report</td>
                                <td>
                                    <div class="flex gap-1">
                                        <button class="btn btn-xs btn-ghost">Edit</button>
                                        <button class="btn btn-xs btn-error">Delete</button>
                                    </div>
                                </td>
                            </tr>
                        </tbody>
                    </table>
                </div>

                <div class="flex justify-between mt-4">
                    <div class="btn-group">
                        <button class="btn btn-sm">«</button>
                        <button class="btn btn-sm">Page 1</button>
                        <button class="btn btn-sm">»</button>
                    </div>
                    <div>
                        <span class="mr-2">Showing 5 of 123 results</span>
                    </div>
                </div>

                <!-- Edit modal placeholder - would be implemented as a modal component -->
                <div class="mt-6 p-4 bg-base-300 rounded-box">
                    <p class="font-bold">Note:</p>
                    <p>Clicking the Edit button would open a modal with a form similar to the "Add Manually" form,
                        pre-filled with the literature's data.</p>
                    <p>The Delete button would prompt for confirmation before removing the item.</p>
                </div>
            </div>
        </div>
    </div>
</template>

<script setup>
import { ref } from 'vue';

// Active tab state
const activeTab = ref('import');
</script>
