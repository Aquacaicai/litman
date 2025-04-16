
from pathlib import Path
import os
from backend.config import Config
from backend.api import api_bp
from flask import Flask, jsonify, render_template, send_from_directory


def create_app(api_root, config_class=Config):
    file_dir = Path(os.path.dirname(os.path.abspath(__file__)))
    template_dir = file_dir.parent / "frontend" / "dist"

    app = Flask(__name__,
                static_folder=str(template_dir),
                template_folder=str(template_dir))
    app.config.from_object(config_class)

    app.register_blueprint(api_bp, url_prefix='/api')

    @app.route('/', defaults={'path': ''})
    @app.route('/<path:path>')
    def catch_all(path):
        static_file_path = os.path.join(app.static_folder, path)
        if os.path.isfile(static_file_path):
            return send_from_directory(app.static_folder, path)
        return render_template("index.html", api_root=api_root)

    @app.errorhandler(500)
    def internal_error(error):
        return jsonify({
            'success': False,
            'message': 'Internal server error'
        }), 500

    return app
