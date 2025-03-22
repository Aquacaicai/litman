import xml.etree.ElementTree as ET
from typing import List, Optional
from models.article import Article


def parse_articles_from_xml(xml_content: str) -> List[Article]:
    xml_content = f"<root>{xml_content}</root>"

    articles = []
    try:
        root = ET.fromstring(xml_content)

        for entry_type in ['article', 'inproceedings', 'proceedings', 'book', 'incollection', 'phdthesis', 'mastersthesis']:
            for entry in root.findall(entry_type):
                article = parse_article_entry(entry, entry_type)
                if article:
                    articles.append(article)
    except ET.ParseError as e:
        raise e

    return articles


def parse_article_entry(entry, entry_type: str) -> Optional[Article]:
    try:
        title_elem = entry.find('title')
        title = title_elem.text if title_elem is not None else "Untitled"

        authors = []
        for author_elem in entry.findall('author'):
            if author_elem.text:
                authors.append(author_elem.text)

        year_elem = entry.find('year')
        year = int(
            year_elem.text) if year_elem is not None and year_elem.text.isdigit() else 0

        # todo: keywords
        keywords = []

        pages_elem = entry.find('pages')
        pages = pages_elem.text if pages_elem is not None else None

        url_elem = entry.find('url')
        url = url_elem.text if url_elem is not None else None

        booktitle_elem = entry.find('booktitle')
        booktitle = booktitle_elem.text if booktitle_elem is not None else None

        ee_elem = entry.find('ee')
        ee = ee_elem.text if ee_elem is not None else None

        key = entry.get('key')

        return Article(
            article_id=None,  # later assigned by storage
            title=title,
            authors=authors,
            year=year,
            keywords=keywords,
            venue=entry_type,
            abstract=None,
            url=url,
            pages=pages,
            booktitle=booktitle,
            ee=ee,
            key=key
        )
    except Exception as e:
        print(f"Failed to parse article: {e}")
        return None
