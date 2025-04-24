from typing import List, Optional
from backend.models.article import Article
from backend.models.storage import LiteratureStorage
from backend.utils.xml_parser import extract_keywords_basic, parse_articles_from_xml


class ImportService:
    def __init__(self, storage: LiteratureStorage):
        self.storage = storage

    def import_from_xml(self, xml_content: str) -> int:
        articles = parse_articles_from_xml(xml_content)
        imported_count = 0

        for article in articles:
            article_id = self.storage.add_article(
                article, save_immediately=False)
            if article_id > 0:
                imported_count += 1

        self.storage._save_indices()
        self.storage._clear_cache()

        return imported_count

    def import_manual_article(self, article_data: dict) -> Optional[int]:
        try:
            article = Article(
                article_id=None,  # assign later
                title=article_data.get('title', ''),
                authors=article_data.get('authors', []),
                year=int(article_data.get('year', 0)),
                editors=article_data.get('editors', []),
                keywords=extract_keywords_basic(article_data.get('title', '')),
                ee=article_data.get('ee'),
                publisher=article_data.get('publisher'),
                isbn=article_data.get('isbn'),
                volume=article_data.get('volume'),
                series=article_data.get('series'),
                school=article_data.get('school'),
                journal=article_data.get('journal'),
                url=article_data.get('url'),
                pages=article_data.get('pages'),
                booktitle=article_data.get('booktitle'),
            )

            return self.storage.add_article(article)
        except Exception as e:
            print(f"Failed to import article: {e}")
            return None
