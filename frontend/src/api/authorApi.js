import request from './request';

export function getAuthorArticles(author) {
    return request({
        url: `/authors/${encodeURIComponent(author)}/articles`,
        method: 'get'
    });
}

export function getAuthorCollaborators(author) {
    return request({
        url: `/authors/${encodeURIComponent(author)}/collaborators`,
        method: 'get'
    });
}
