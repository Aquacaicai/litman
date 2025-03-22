import os
from models.storage import LiteratureStorage


class Config:
    SECRET_KEY = os.environ.get('SECRET_KEY') or 'hard-to-guess-string'
    DATA_DIR = os.environ.get('DATA_DIR') or 'data'
    B_PLUS_TREE_ORDER = 64


storage = LiteratureStorage(
    storage_dir=Config.DATA_DIR,
    order=Config.B_PLUS_TREE_ORDER
)
