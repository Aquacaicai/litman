import request from './request';

export function getAuthorArticleCounts(limit = 100) {
    return request({
        url: '/stats/authors/article-counts',
        method: 'get',
        params: { limit }
    });
}

export function getYearlyKeywordFrequencies() {
    return request({
        url: '/stats/keywords/yearly',
        method: 'get'
    });
}

export function getCollaborationNetwork() {
    return request({
        url: '/stats/collaboration/network',
        method: 'get'
    });
}

export function getCompleteSubgraphs() {
    return request({
        url: '/stats/collaboration/complete-subgraphs',
        method: 'get'
    });
}

export function getCollaborationGraphData() {
    return request({
        url: '/stats/visualization/collaboration-graph',
        method: 'get'
    });
}
