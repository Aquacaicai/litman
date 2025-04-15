from typing import List, Dict, Any


class Article:
    def __init__(self,
                 article_id: int,
                 title: str,
                 authors: List[str] = None,
                 year: int = None,
                 editors: List[str] = None,
                 keywords: List[str] = None,
                 ee: str = None,
                 publisher: str = None,
                 isbn: str = None,
                 volume: str = None,
                 series: str = None,
                 school: str = None,
                 journal: str = None,
                 url: str = None,
                 pages: str = None,
                 booktitle: str = None):

        self.article_id = article_id
        self.title = title
        self.keywords = keywords
        self.ee = ee
        self.year = year
        self.authors = authors
        self.booktitle = booktitle
        self.url = url
        self.editors = editors
        self.pages = pages
        self.publisher = publisher
        self.isbn = isbn
        self.volume = volume
        self.series = series
        self.school = school
        self.journal = journal

    def to_dict(self) -> Dict[str, Any]:
        return {
            "article_id": self.article_id,
            "title": self.title,
            "keywords": self.keywords,
            "ee": self.ee,
            "year": self.year,
            "authors": self.authors,
            "booktitle": self.booktitle,
            "url": self.url,
            "editors": self.editors,
            "pages": self.pages,
            "publisher": self.publisher,
            "isbn": self.isbn,
            "volume": self.volume,
            "series": self.series,
            "school": self.school,
            "journal": self.journal,
        }

    @staticmethod
    def from_dict(data: Dict[str, Any]) -> 'Article':
        return Article(
            article_id=data.get("article_id"),
            title=data.get("title"),
            keywords=data.get("keywords"),
            ee=data.get("ee"),
            year=data.get("year"),
            authors=data.get("authors"),
            booktitle=data.get("booktitle"),
            url=data.get("url"),
            editors=data.get("editors"),
            pages=data.get("pages"),
            publisher=data.get("publisher"),
            isbn=data.get("isbn"),
            volume=data.get("volume"),
            series=data.get("series"),
            school=data.get("school"),
            journal=data.get("journal"),
        )
