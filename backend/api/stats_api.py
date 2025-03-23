from flask import jsonify, request
from api import api_bp
from services.stats_service import StatsService
from config import storage

stats_service = StatsService(storage)


@api_bp.route('/stats/authors/article-counts', methods=['GET'])
def get_author_article_counts():
    limit = request.args.get('limit', default=100, type=int)

    counts = stats_service.get_author_article_counts()
    counts = counts[:limit]

    return jsonify({
        'success': True,
        'data': [{'author': author, 'count': count} for author, count in counts]
    })


@api_bp.route('/stats/keywords/yearly', methods=['GET'])
def get_yearly_keyword_frequencies():
    data = stats_service.get_yearly_keyword_frequencies()

    formatted_data = {}
    for year, keywords in data.items():
        formatted_data[year] = [
            {'keyword': kw, 'frequency': freq} for kw, freq in keywords]

    return jsonify({
        'success': True,
        'data': formatted_data
    })


@api_bp.route('/stats/collaboration/network', methods=['GET'])
def get_collaboration_network():
    network = stats_service.get_collaboration_network()

    # to json
    formatted_network = []
    for author, collaborators in network.items():
        formatted_network.append({
            'author': author,
            'collaborators': list(collaborators)
        })

    return jsonify({
        'success': True,
        'data': formatted_network
    })


@api_bp.route('/stats/collaboration/complete-subgraphs', methods=['GET'])
def get_complete_subgraphs():
    subgraphs = stats_service.count_complete_subgraphs()

    formatted_data = [{'order': order, 'count': count}
                      for order, count in subgraphs.items()]

    return jsonify({
        'success': True,
        'data': formatted_data
    })


@api_bp.route('/stats/visualization/collaboration-graph', methods=['GET'])
def get_visualization_data():
    network = stats_service.get_collaboration_network()

    nodes = []
    links = []
    author_ids = {}  # autoor name -> node id

    # nodes
    for i, author in enumerate(network.keys()):
        author_ids[author] = i
        nodes.append({
            'id': i,
            'name': author,
            'article_count': len(storage.get_articles_by_author(author))
        })

    # edges
    for author, collaborators in network.items():
        source_id = author_ids[author]
        for collaborator in collaborators:
            if collaborator in author_ids:
                target_id = author_ids[collaborator]
                # dup edge
                if source_id < target_id:
                    links.append({
                        'source': source_id,
                        'target': target_id
                    })

    return jsonify({
        'success': True,
        'data': {
            'nodes': nodes,
            'links': links
        }
    })
