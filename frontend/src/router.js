import { createRouter, createWebHistory } from 'vue-router';
import LiteratureSearch from './components/LiteratureSearch.vue';
import StatisticsView from './components/StatisticsView.vue';
import CollaborationsView from './components/CollaborationsView.vue';
import ManageLiterature from './components/ManageLiterature.vue';
import ArticleDetail from './components/ArticleDetail.vue';

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
    },
    {
        path: '/article/:id',
        name: 'ArticleDetail',
        component: ArticleDetail,
        props: true
    }
];

const router = createRouter({
    history: createWebHistory(),
    routes
});

export default router;
