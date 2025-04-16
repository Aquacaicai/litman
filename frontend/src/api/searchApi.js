import request from './request';

export function searchByTitle(query) {
    return request({
        url: '/search/title',
        method: 'get',
        params: { q: query }
    });
}
