import { createRouter, createWebHistory } from 'vue-router';
import LiteratureSearch from './components/LiteratureSearch.vue';
import StatisticsView from './components/StatisticsView.vue';
import CollaborationsView from './components/CollaborationsView.vue';
import ManageLiterature from './components/ManageLiterature.vue';

const routes = [
    {
        path: '/',
        name: 'LiteratureSearch',
        component: LiteratureSearch
    },
    {
        path: '/statistics',
        name: 'Statistics',
        component: StatisticsView
    },
    {
        path: '/collaborations',
        name: 'Collaborations',
        component: CollaborationsView
    },
    {
        path: '/manage',
        name: 'ManageLiterature',
        component: ManageLiterature
    }
];

const router = createRouter({
    history: createWebHistory(),
    routes
});

export default router;
