from typing import List, Dict, Any


class Article:
    def __init__(self,
                 article_id: int,
                 title: str,
                 authors: List[str],
                 year: int,
                 keywords: List[str] = None,
                 venue: str = None,
                 abstract: str = None,
                 url: str = None,
                 pages: str = None,
                 booktitle: str = None,
                 ee: str = None,
                 key: str = None):
        self.article_id = article_id
        self.title = title
        self.authors = authors
        self.year = year
        self.keywords = keywords or []
        self.venue = venue
        self.abstract = abstract
        self.url = url
        self.pages = pages
        self.booktitle = booktitle
        self.ee = ee
        self.key = key

    def to_dict(self) -> Dict[str, Any]:
        return {
            "article_id": self.article_id,
            "title": self.title,
            "authors": self.authors,
            "year": self.year,
            "keywords": self.keywords,
            "venue": self.venue,
            "abstract": self.abstract,
            "url": self.url,
            "pages": self.pages,
            "booktitle": self.booktitle,
            "ee": self.ee,
            "key": self.key
        }

    @staticmethod
    def from_dict(data: Dict[str, Any]) -> 'Article':
        return Article(
            article_id=data.get("article_id"),
            title=data.get("title"),
            authors=data.get("authors", []),
            year=data.get("year"),
            keywords=data.get("keywords", []),
            venue=data.get("venue"),
            abstract=data.get("abstract"),
            url=data.get("url"),
            pages=data.get("pages"),
            booktitle=data.get("booktitle"),
            ee=data.get("ee"),
            key=data.get("key")
        )
