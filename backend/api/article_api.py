from flask import jsonify, request
from api import api_bp
from services.search_service import SearchService
from models.storage import LiteratureStorage
from config import storage

search_service = SearchService(storage)


@api_bp.route('/articles/<int:article_id>', methods=['GET'])
def get_article(article_id):
    article = search_service.get_article_by_id(article_id)
    if article:
        return jsonify({
            'success': True,
            'data': article.to_dict()
        })
    else:
        return jsonify({
            'success': False,
            'message': f'Article {article_id} not found'
        }), 404


@api_bp.route('/articles/title/<string:title>', methods=['GET'])
def get_article_by_title(title):
    article = search_service.get_article_by_title(title)
    if article:
        return jsonify({
            'success': True,
            'data': article.to_dict()
        })
    else:
        return jsonify({
            'success': False,
            'message': f'Article "{title}" not found'
        }), 404
