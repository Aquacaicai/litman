import os
import pickle
from typing import List, Dict, Optional, Tuple, Any, Set
import random
from bptree import BPTreeIntInt, BPTreeIntStr, BPTreeIntVecInt, BPTreeStrInt, BPTreeStrStr, BPTreeStrVecInt
from backend.models.article import Article
from thefuzz import process

# 256MB
MAX_FILE_SIZE = 256 * 1024 * 1024


class LiteratureStorage:
    def __init__(self, storage_dir: str, order: int = 64):
        self.storage_dir = storage_dir
        self.binary_dir = os.path.join(storage_dir, "binary")
        self.index_dir = os.path.join(storage_dir, "index")

        os.makedirs(self.binary_dir, exist_ok=True)
        os.makedirs(self.index_dir, exist_ok=True)

        # literature_id -> (filename, offset, len)
        self.main_index = BPTreeIntStr(order)
        # author -> [literature_id]
        self.author_index = BPTreeStrVecInt(order)
        # title -> literature_id
        self.title_index = BPTreeStrInt(order)
        # year -> [literature_id]
        self.date_index = BPTreeIntVecInt(order)

        self._load_indices()
        self.max_article_id = self._get_max_article_id()
        self.current_bin_file = self._get_current_bin_file()

    def _load_indices(self):
        main_index_file = os.path.join(self.index_dir, "main_index.dat")
        author_index_file = os.path.join(self.index_dir, "author_index.dat")
        title_index_file = os.path.join(self.index_dir, "title_index.dat")
        date_index_file = os.path.join(self.index_dir, "date_index.dat")

        if os.path.exists(main_index_file):
            self.main_index.deserialize(main_index_file)
        if os.path.exists(author_index_file):
            self.author_index.deserialize(author_index_file)
        if os.path.exists(title_index_file):
            self.title_index.deserialize(title_index_file)
        if os.path.exists(date_index_file):
            self.date_index.deserialize(date_index_file)

    def _save_indices(self):
        self.main_index.serialize(os.path.join(
            self.index_dir, "main_index.dat"))
        self.author_index.serialize(os.path.join(
            self.index_dir, "author_index.dat"))
        self.title_index.serialize(os.path.join(
            self.index_dir, "title_index.dat"))
        self.date_index.serialize(os.path.join(
            self.index_dir, "date_index.dat"))

    def _get_max_article_id(self) -> int:
        max_id = 0
        bin_files = [f for f in os.listdir(
            self.binary_dir) if f.startswith("articles_")]
        if not bin_files:
            return 0

        for file_name in bin_files:
            parts = file_name.split('_')
            if len(parts) >= 3:
                try:
                    end_id = int(parts[2].split('.')[0])
                    max_id = max(max_id, end_id)
                except ValueError:
                    continue

        return max_id

    def _get_current_bin_file(self) -> str:
        bin_files = [f for f in os.listdir(
            self.binary_dir) if f.startswith("articles_")]

        if not bin_files:
            return os.path.join(self.binary_dir, "articles_1.bin")

        latest_file = sorted(bin_files, key=lambda f: int(
            f.split('_')[1].split('.')[0]))[-1]
        file_path = os.path.join(self.binary_dir, latest_file)

        if os.path.getsize(file_path) >= MAX_FILE_SIZE:
            start_id = int(latest_file.split('_')[2].split('.')[0]) + 1
            new_file = f"articles_{start_id}.bin"
            return os.path.join(self.binary_dir, new_file)

        return file_path

    def add_article(self, article: Article, save_immediately=True) -> int:
        if article.article_id is None:
            self.max_article_id += 1
            article.article_id = self.max_article_id

        article_data = pickle.dumps(article.to_dict())

        current_file_path = self.current_bin_file
        if os.path.exists(current_file_path) and os.path.getsize(current_file_path) + len(article_data) > MAX_FILE_SIZE:
            # new file
            start_id = article.article_id
            new_file = f"articles_{start_id}.bin"
            self.current_bin_file = os.path.join(self.binary_dir, new_file)
            current_file_path = self.current_bin_file

        # write data
        with open(current_file_path, 'ab') as f:
            offset = f.tell()
            f.write(article_data)

        rel_path = os.path.relpath(current_file_path, self.binary_dir)

        # update main index
        location_info = f"{rel_path},{offset},{len(article_data)}"
        self.main_index.insert(article.article_id, location_info)

        # update author index
        for author in article.authors:
            author_articles = self.author_index.find(author)
            if author_articles is None:
                self.author_index.insert(author, [article.article_id])
            else:
                if article.article_id not in author_articles:
                    author_articles.append(article.article_id)
                    self.author_index.update(author, author_articles)

        # update title index
        if self.title_index.find(article.title) is None:
            self.title_index.insert(article.title, article.article_id)
        else:
            original_title = article.title
            new_title = f"{original_title} - dup id({article.article_id})"

            article.title = new_title

            self.title_index.insert(new_title, article.article_id)

        # update date index
        year_articles = self.date_index.find(article.year)
        if year_articles is None:
            self.date_index.insert(article.year, [article.article_id])
        else:
            if article.article_id not in year_articles:
                year_articles.append(article.article_id)
                self.date_index.update(article.year, year_articles)

        if save_immediately:
            self._save_indices()
        return article.article_id

    def get_article_by_id(self, article_id: int) -> Optional[Article]:
        location_info = self.main_index.find(article_id)
        if location_info is None:
            return None

        file_path, offset, length = location_info.split(',')
        offset, length = int(offset), int(length)

        full_path = os.path.join(self.binary_dir, file_path)

        with open(full_path, 'rb') as f:
            f.seek(offset)
            article_data = f.read(length)

        try:
            article_dict = pickle.loads(article_data)
            return Article.from_dict(article_dict)
        except Exception as e:
            print(f"Error loading article {article_id}: {e}")
            return None

    def get_articles_by_author(self, author: str) -> List[Article]:
        article_ids = self.author_index.find(author)
        if article_ids is None:
            return []

        return [self.get_article_by_id(article_id) for article_id in article_ids]

    def get_article_by_title(self, title: str) -> Optional[Article]:
        article_id = self.title_index.find(title)
        if article_id is None:
            return None

        return self.get_article_by_id(article_id)

    def get_collaborators(self, author: str) -> Set[str]:
        articles = self.get_articles_by_author(author)
        collaborators = set()

        for article in articles:
            for coauthor in article.authors:
                if coauthor != author:
                    collaborators.add(coauthor)

        return collaborators

    def search_articles_by_title(self, title_pattern: str) -> List[Article]:
        all_titles = self.title_index.getAllKeys()

        matches = process.extractBests(
            title_pattern.lower(), all_titles, score_cutoff=60, limit=None)

        matched_articles = []
        for matched_title, score in matches:
            article_id = self.title_index.find(
                matched_title)
            if article_id:
                article = self.get_article_by_id(article_id)
                if article:
                    matched_articles.append(article)

        return matched_articles

    def get_author_article_counts(self) -> Dict[str, int]:
        # brute force count, to be optimized
        # todo: b+ tree for author -> count
        author_counts = {}

        all_authors = set()
        for article_id in range(1, self.max_article_id + 1):
            article = self.get_article_by_id(article_id)
            if article:
                for author in article.authors:
                    all_authors.add(author)

        for author in all_authors:
            articles = self.author_index.find(author)
            if articles:
                author_counts[author] = len(articles)

        return author_counts

    def get_yearly_keyword_frequencies(self) -> Dict[int, Dict[str, int]]:
        # brute force count, to be optimized
        yearly_keywords = {}

        # all articles
        for article_id in range(1, self.max_article_id + 1):
            article = self.get_article_by_id(article_id)
            if article and article.keywords:
                if article.year not in yearly_keywords:
                    yearly_keywords[article.year] = {}

                # all keywords
                for keyword in article.keywords:
                    if keyword not in yearly_keywords[article.year]:
                        yearly_keywords[article.year][keyword] = 0
                    yearly_keywords[article.year][keyword] += 1

        return yearly_keywords

    def get_collaboration_graph(self) -> Dict[str, Set[str]]:
        collaboration_graph = {}

        for article_id in range(1, self.max_article_id + 1):
            article = self.get_article_by_id(article_id)
            if article and len(article.authors) > 1:
                for author in article.authors:
                    if author not in collaboration_graph:
                        collaboration_graph[author] = set()

                    for coauthor in article.authors:
                        if coauthor != author:
                            collaboration_graph[author].add(coauthor)

        return collaboration_graph

    def count_complete_subgraphs(self, graph: Dict[str, Set[str]]) -> Dict[int, int]:
        # todo: optimize

        complete_subgraphs = {2: 0, 3: 0}

        # 2
        edge_count = 0
        for author, collaborators in graph.items():
            edge_count += len(collaborators)
        # div 2
        complete_subgraphs[2] = edge_count // 2

        # 3
        triangle_count = 0
        authors = list(graph.keys())
        for i in range(len(authors)):
            for j in range(i+1, len(authors)):
                if authors[j] in graph[authors[i]]:  # i-j
                    for k in range(j+1, len(authors)):
                        if authors[k] in graph[authors[i]] and authors[k] in graph[authors[j]]:
                            # -i-j-k
                            triangle_count += 1

        complete_subgraphs[3] = triangle_count

        return complete_subgraphs
