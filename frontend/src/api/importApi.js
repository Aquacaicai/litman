import request from './request';

export function importFromXml(xmlContent) {
    return request({
        url: '/import/xml',
        method: 'post',
        data: xmlContent,
        headers: {
            'Content-Type': 'text/xml'
        }
    });
}

export function importManualArticle(articleData) {
    return request({
        url: '/import/manual',
        method: 'post',
        data: articleData
    });
}
