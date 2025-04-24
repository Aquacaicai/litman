from flask import jsonify, request
from backend.api import api_bp
from backend.services.import_service import ImportService
from backend.config import storage

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
            'message': 'Empty data received'
        }), 400

    # Validate required fields
    if 'title' not in data or not data['title'].strip():
        return jsonify({
            'success': False,
            'message': 'Title is required'
        }), 400

    if 'authors' not in data or not data['authors'] or not isinstance(data['authors'], list):
        return jsonify({
            'success': False,
            'message': 'Authors must be provided as a non-empty list'
        }), 400

    if 'year' in data and data['year']:
        try:
            year = int(data['year'])
            if year <= 0:
                return jsonify({
                    'success': False,
                    'message': 'Year must be a positive integer'
                }), 400
        except ValueError:
            return jsonify({
                'success': False,
                'message': 'Year must be a valid integer'
            }), 400

    try:
        if isinstance(data.get('authors', []), str):
            data['authors'] = [data['authors']]

        if isinstance(data.get('editors', []), str):
            data['editors'] = [data['editors']]

        article_id = import_service.import_manual_article(data)

        if article_id:
            return jsonify({
                'success': True,
                'message': 'Article imported successfully',
                'article_id': article_id
            }), 201
        else:
            return jsonify({
                'success': False,
                'message': 'Failed to import article'
            }), 500
    except Exception as e:
        return jsonify({
            'success': False,
            'message': f'Error importing article: {str(e)}'
        }), 500
