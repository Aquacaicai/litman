from flask import jsonify, request
from backend.api import api_bp
from backend.services.search_service import SearchService
from backend.config import storage

search_service = SearchService(storage)


@api_bp.route('/search/title', methods=['GET'])
def search_by_title():
    query = request.args.get('q', '')
    if not query:
        return jsonify({
            'success': False,
            'message': 'Empty keyword'
        }), 400

    articles = search_service.search_articles_by_title(query)
    return jsonify({
        'success': True,
        'data': [article.to_dict() for article in articles]
    })
