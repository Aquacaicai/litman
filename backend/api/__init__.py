from flask import Blueprint

api_bp = Blueprint('api', __name__)

# fmt: off
from . import article_api, author_api, search_api, stats_api, import_api
# fmt: on