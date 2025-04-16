from typing import List, Optional
from backend.models.article import Article
from nltk.corpus import stopwords
from nltk.tokenize import word_tokenize
from lxml import etree


def parse_articles_from_xml(xml_content: str) -> List[Article]:
    articles = []
    try:
        parser = etree.XMLParser(recover=True)
        root = etree.fromstring(xml_content.encode('utf-8'), parser)

        for entry_type in ['article',
                           'inproceedings',
                           'proceedings',
                           'book',
                           'incollection',
                           'phdthesis',
                           'mastersthesis']:
            for entry in root.findall(entry_type):
                article = parse_article_entry(entry, entry_type)
                if article:
                    articles.append(article)
    except Exception as e:
        raise e

    return articles


def parse_article_entry(entry, entry_type: str) -> Optional[Article]:
    try:
        # title, ee, year, author, booktitle, url, editor, pages, publisher, isbn, volume, series, school, journal
        title_elem = entry.find('title')
        title = title_elem.text if title_elem is not None else "Untitled"

        title = title if title is not None else "Untitled"
        keywords = extract_keywords_basic(title) if title != "Untitled" else []

        ee_elem = entry.find('ee')
        ee = ee_elem.text if ee_elem is not None else None

        year_elem = entry.find('year')
        year = int(
            year_elem.text) if year_elem is not None and year_elem.text.isdigit() else 0

        authors = []
        for author_elem in entry.findall('author'):
            if author_elem.text:
                authors.append(author_elem.text)

        booktitle_elem = entry.find('booktitle')
        booktitle = booktitle_elem.text if booktitle_elem is not None else None

        url_elem = entry.find('url')
        url = url_elem.text if url_elem is not None else None

        editors = []
        for editor_elem in entry.findall('editor'):
            if editor_elem.text:
                editors.append(editor_elem.text)

        pages_elem = entry.find('pages')
        pages = pages_elem.text if pages_elem is not None else None

        publisher_elem = entry.find('publisher')
        publisher = publisher_elem.text if publisher_elem is not None else None

        isbn_elem = entry.find('isbn')
        isbn = isbn_elem.text if isbn_elem is not None else None

        volume_elem = entry.find('volume')
        volume = volume_elem.text if volume_elem is not None else None

        series_elem = entry.find('series')
        series = series_elem.text if series_elem is not None else None

        school_elem = entry.find('school')
        school = school_elem.text if school_elem is not None else None

        journal_elem = entry.find('journal')
        journal = journal_elem.text if journal_elem is not None else None

        return Article(
            article_id=None,  # later assigned by storage
            title=title,
            keywords=keywords,
            ee=ee,
            year=year,
            authors=authors,
            booktitle=booktitle,
            url=url,
            editors=editors,
            pages=pages,
            publisher=publisher,
            isbn=isbn,
            volume=volume,
            series=series,
            school=school,
            journal=journal,
        )
    except Exception as e:
        print(f"Failed to parse article: {e}")
        return None


def extract_keywords_basic(title):
    tokens = word_tokenize(title.lower())

    stop_words = set(stopwords.words('english'))
    keywords = [word for word in tokens if word.isalnum()
                and word not in stop_words]

    return keywords
