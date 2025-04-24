<script setup>
import { ref, computed } from 'vue';
import api from '@/api';

// Active tab state
const activeTab = ref('import');

const fileInput = ref(null);
const fileContent = ref(null);
const article = ref({
    title: '',
    authors: [],
    year: null,
    editors: [],
    ee: '',
    publisher: '',
    isbn: '',
    volume: '',
    series: '',
    school: '',
    journal: '',
    url: '',
    pages: '',
    booktitle: ''
});

const authorsInput = ref('');
const editorsInput = ref('');
const isFormValid = computed(() => {
    return article.value.title &&
        article.value.authors.length > 0 &&
        article.value.year;
});


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


const parseStringToArray = (input, separator = ';') => {
    if (!input) return [];
    return input.split(separator)
        .map(item => item.trim())
        .filter(item => item.length > 0);
};

const updateArrayFields = () => {
    article.value.authors = parseStringToArray(authorsInput.value);
    article.value.editors = parseStringToArray(editorsInput.value);
};
async function addLiterature() {
    updateArrayFields();

    if (!isFormValid.value) {
        // toast.error("Please fill in all required fields (Title, Authors, Year)");
        return;
    }

    try {
        api.import.importManualArticle(article.value)
        // toast.success("Literature added successfully!");
        resetForm();
    } catch (error) {
        console.error("Error adding literature:", error);
        // toast.error("Failed to add literature. Please try again.");
    }
};
function resetForm() {
    article.value = {
        title: '',
        authors: [],
        year: null,
        editors: [],
        ee: '',
        publisher: '',
        isbn: '',
        volume: '',
        series: '',
        school: '',
        journal: '',
        url: '',
        pages: '',
        booktitle: ''
    };

    authorsInput.value = '';
    editorsInput.value = '';
};
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
                        <span class="label-text">Title *</span>
                    </label>
                    <input type="text" placeholder="Enter title" class="input input-bordered" v-model="article.title" />
                </div>



                <div class="grid grid-cols-1 md:grid-cols-2 gap-4">
                    <div class="form-control">
                        <label class="label">
                            <span class="label-text">Authors *</span>
                            <span class="label-text-alt">Separated by semicolons (;)</span>
                        </label>
                        <input type="text" placeholder="Author1; Author2; Author3" class="input input-bordered"
                            v-model="authorsInput" />
                    </div>
                    <div class="form-control">
                        <label class="label">
                            <span class="label-text">Editors</span>
                            <span class="label-text-alt">Separated by semicolons (;)</span>
                        </label>
                        <input type="text" placeholder="Editor1; Editor2" class="input input-bordered"
                            v-model="editorsInput" />
                    </div>
                </div>

                <div class="grid grid-cols-1 md:grid-cols-3 gap-4">
                    <div class="form-control">
                        <label class="label">
                            <span class="label-text">Year *</span>
                        </label>
                        <input type="number" placeholder="YYYY" class="input input-bordered" v-model="article.year" />
                    </div>

                    <div class="form-control">
                        <label class="label">
                            <span class="label-text">Volume</span>
                        </label>
                        <input type="text" placeholder="Volume number" class="input input-bordered"
                            v-model="article.volume" />
                    </div>

                    <div class="form-control">
                        <label class="label">
                            <span class="label-text">Pages</span>
                        </label>
                        <input type="text" placeholder="e.g., 123-145" class="input input-bordered"
                            v-model="article.pages" />
                    </div>
                </div>

                <div class="grid grid-cols-1 md:grid-cols-2 gap-4">
                    <div class="form-control">
                        <label class="label">
                            <span class="label-text">Journal</span>
                        </label>
                        <input type="text" placeholder="Journal name" class="input input-bordered"
                            v-model="article.journal" />
                    </div>

                    <div class="form-control">
                        <label class="label">
                            <span class="label-text">Book Title</span>
                        </label>
                        <input type="text" placeholder="For book chapters or proceedings" class="input input-bordered"
                            v-model="article.booktitle" />
                    </div>
                </div>

                <div class="grid grid-cols-1 md:grid-cols-2 gap-4">
                    <div class="form-control">
                        <label class="label">
                            <span class="label-text">Publisher</span>
                        </label>
                        <input type="text" placeholder="Publisher name" class="input input-bordered"
                            v-model="article.publisher" />
                    </div>

                    <div class="form-control">
                        <label class="label">
                            <span class="label-text">School/Institution</span>
                        </label>
                        <input type="text" placeholder="For theses or technical reports" class="input input-bordered"
                            v-model="article.school" />
                    </div>
                </div>

                <div class="grid grid-cols-1 md:grid-cols-2 gap-4">
                    <div class="form-control">
                        <label class="label">
                            <span class="label-text">Series</span>
                        </label>
                        <input type="text" placeholder="Book/conference series" class="input input-bordered"
                            v-model="article.series" />
                    </div>

                    <div class="form-control">
                        <label class="label">
                            <span class="label-text">ISBN</span>
                        </label>
                        <input type="text" placeholder="ISBN number" class="input input-bordered"
                            v-model="article.isbn" />
                    </div>
                </div>

                <div class="grid grid-cols-1 md:grid-cols-2 gap-4">
                    <div class="form-control">
                        <label class="label">
                            <span class="label-text">URL</span>
                        </label>
                        <input type="url" placeholder="https://example.com/paper" class="input input-bordered"
                            v-model="article.url" />
                    </div>

                    <div class="form-control">
                        <label class="label">
                            <span class="label-text">DOI/Electronic Edition</span>
                        </label>
                        <input type="text" placeholder="DOI or electronic reference" class="input input-bordered"
                            v-model="article.ee" />
                    </div>
                </div>

                <div class="flex justify-end mt-6 gap-2">
                    <button class="btn btn-ghost" @click="resetForm">Reset</button>
                    <button class="btn btn-primary" @click="addLiterature">Add Literature</button>
                </div>
            </div>
        </div>

    </div>
</template>
