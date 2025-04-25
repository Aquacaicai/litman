import request from './request';

export function searchByKeyword(query) {
    return request({
        url: '/search/keywords',
        method: 'get',
        params: { q: query }
    });
}
