from flask import jsonify, request
from api import api_bp
from services.import_service import ImportService
from config import storage

import_service = ImportService(storage)


@api_bp.route('/import/xml', methods=['POST'])
def import_from_xml():
    if not request.data:
        return jsonify({
            'success': False,
            'message': 'Empty XML'
        }), 400

    try:
        xml_content = request.data.decode('utf-8')
        imported_count = import_service.import_from_xml(xml_content)

        return jsonify({
            'success': True,
            'message': f'{imported_count} articles imported',
            'count': imported_count
        })
    except Exception as e:
        return jsonify({
            'success': False,
            'message': f'Failed to import: {str(e)}'
        }), 500


@api_bp.route('/import/manual', methods=['POST'])
def import_manual_article():
    data = request.json
    if not data:
        return jsonify({
            'success': False,
            'message': 'Empty data'
        }), 400

    if 'title' not in data or not data['title']:
        return jsonify({
            'success': False,
            'message': 'Empty title'
        }), 400

    if 'authors' not in data or not data['authors']:
        return jsonify({
            'success': False,
            'message': 'Empty authors'
        }), 400

    try:
        article_id = import_service.import_manual_article(data)
        if article_id:
            return jsonify({
                'success': True,
                'message': 'Import success',
                'article_id': article_id
            })
        else:
            return jsonify({
                'success': False,
                'message': 'Import failed'
            }), 500
    except Exception as e:
        return jsonify({
            'success': False,
            'message': f'Failed to import: {str(e)}'
        }), 500
