from typing import List, Dict, Optional, Set
from backend.models.article import Article
from backend.models.storage import LiteratureStorage


class SearchService:
    def __init__(self, storage: LiteratureStorage):
        self.storage = storage

    def get_articles_by_author(self, author: str) -> List[Article]:
        return self.storage.get_articles_by_author(author)

    def get_article_by_title(self, title: str) -> Optional[Article]:
        return self.storage.get_article_by_title(title)

    def get_collaborators(self, author: str) -> Set[str]:
        return self.storage.get_collaborators(author)

    def search_articles_by_title(self, title_pattern: str) -> List[Article]:
        return self.storage.search_articles_by_title(title_pattern)

    def get_article_by_id(self, article_id: int) -> Optional[Article]:
        return self.storage.get_article_by_id(article_id)
