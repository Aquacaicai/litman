<script setup>
import { ref } from 'vue';
import api from '@/api';

// Active tab state
const activeTab = ref('import');

const fileInput = ref(null);
const fileContent = ref(null);

function handleFileChange(event) {
    const file = event.target.files[0];
    if (file && file.type === 'text/xml' || file.type === 'application/xml' || file.name.endsWith('.xml')) {
        const reader = new FileReader();
        reader.onload = (e) => {
            fileContent.value = e.target.result;
        };
        reader.readAsText(file);
    } else {
        alert('Please select a valid XML file');
        event.target.value = '';
        fileContent.value = null;
    }
}
function handleImportLiteratureClick() {
    if (!fileContent.value) {
        alert('Please select an XML file first');
        return;
    }

    api.import.importFromXml(fileContent.value)
        .then(response => {
            alert('Import successful!');
            console.log('Import response:', response);
        })
        .catch(error => {
            alert('Import failed: ' + (error.message || 'Unknown error'));
            console.error('Import error:', error);
        });
}

</script>


<template>
    <div>
        <h2 class="text-2xl font-bold mb-6">Manage Literature</h2>

        <div class="tabs tabs-boxed mb-6">
            <a class="tab" :class="{ 'tab-active': activeTab === 'import' }" @click="activeTab = 'import'">Import
                Literature</a>
            <a class="tab" :class="{ 'tab-active': activeTab === 'add' }" @click="activeTab = 'add'">Add Manually</a>
        </div>

        <!-- Import Literature -->
        <div v-if="activeTab === 'import'" class="card bg-base-100 shadow-xl">
            <div class="card-body">
                <h2 class="card-title">Import from XML</h2>

                <div class="form-control">
                    <label class="label">
                        <span class="label-text">Upload XML File</span>
                    </label>
                    <input type="file" ref="fileInput" @change="handleFileChange" accept=".xml,application/xml,text/xml"
                        class="file-input file-input-bordered w-full" />
                    <label class="label">
                        <span class="label-text-alt">Supported formats: XML files with literature metadata</span>
                    </label>
                </div>
                <div class="flex justify-end mt-4">
                    <button class="btn btn-primary" @click="handleImportLiteratureClick">Import Literature</button>
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
    </div>
</template>
