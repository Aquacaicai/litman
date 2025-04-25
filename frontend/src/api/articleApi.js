import request from './request'

export function getArticle(articleId) {
  return request({
    url: `/articles/${articleId}`,
    method: 'get',
  })
}

export function getArticleByTitle(title) {
  return request({
    url: `/articles/title/${encodeURIComponent(title)}`,
    method: 'get',
  })
}
