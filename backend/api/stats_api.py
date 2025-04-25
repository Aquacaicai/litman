from flask import jsonify, request
from backend.api import api_bp
from backend.services.stats_service import StatsService
from backend.config import storage
from flask import Response, stream_with_context
import json
import time

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
    limit = request.args.get('limit', default=10, type=int)
    data = stats_service.get_yearly_keyword_frequencies(limit=limit)

    formatted_data = {}
    for year, keywords in data.items():
        formatted_data[year] = [
            {'keyword': kw, 'frequency': freq} for kw, freq in keywords]

    return jsonify({
        'success': True,
        'data': formatted_data
    })


@api_bp.route('/stats/collaboration/cliques-counts', methods=['GET'])
def get_cliques_counts():
    return Response(stream_with_context(generate_cliques_counts()),
                    content_type='text/event-stream')


def generate_cliques_counts():
    yield f"data: {json.dumps({'status': 'started', 'progress': 0})}\n\n"

    try:
        class ProgressState:
            def __init__(self):
                self.current_message = None
                self.message_available = False

            def set_message(self, status, current, total):
                progress = (current / total) * 100 if total > 0 else 0
                self.current_message = f"data: {json.dumps({'status': status, 'progress': progress})}\n\n"
                self.message_available = True

            def get_message(self):
                if self.message_available:
                    self.message_available = False
                    return self.current_message
                return None

        progress_state = ProgressState()

        def progress_callback(status, current, total):
            progress_state.set_message(status, current, total)

        import threading
        result = {'data': None}

        def processing_thread():
            try:
                result['data'] = stats_service.count_cliques_with_progress(
                    progress_callback)
            except Exception as e:
                progress_state.set_message('error', 0, 0)
                print(f"Error in processing thread: {e}")

        thread = threading.Thread(target=processing_thread)
        thread.daemon = True
        thread.start()

        while thread.is_alive():
            message = progress_state.get_message()
            if message:
                yield message
            import time
            time.sleep(1)

        message = progress_state.get_message()
        if message:
            yield message

        if result['data']:
            subgraphs = result['data']
            formatted_data = [{'order': order, 'count': count}
                              for order, count in subgraphs.items()]
            yield f"data: {json.dumps({'status': 'done', 'data': formatted_data})}\n\n"
        else:
            yield f"data: {json.dumps({'status': 'error', 'data': 'Processing failed'})}\n\n"

    except Exception as e:
        yield f"data: {json.dumps({'status': 'error', 'data': str(e)})}\n\n"
