import os
import pickle
from typing import List, Dict, Optional, Tuple, Any, Set
from bptree import BPTreeIntStr, BPTreeIntVecInt, BPTreeWStrInt, BPTreeWStrVecInt
from backend.models.article import Article
from backend.utils.xml_parser import extract_keywords_basic
from pivoter import pivoter
from functools import lru_cache
from cachetools import cached

import tqdm
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
        self.author_index = BPTreeWStrVecInt(order)
        # title -> literature_id
        self.title_index = BPTreeWStrInt(order)
        # keyword -> [literature_id]
        self.keyword_index = BPTreeWStrVecInt(order)
        # year -> [literature_id]
        self.date_index = BPTreeIntVecInt(order)

        self._load_indices()
        self.max_article_id = self._get_max_article_id()
        self.current_bin_file = self._get_current_bin_file()

    def _load_indices(self):
        main_index_file = os.path.join(self.index_dir, "main_index.dat")
        author_index_file = os.path.join(self.index_dir, "author_index.dat")
        title_index_file = os.path.join(self.index_dir, "title_index.dat")
        keyword_index_file = os.path.join(self.index_dir, "keyword_index.dat")
        date_index_file = os.path.join(self.index_dir, "date_index.dat")

        if os.path.exists(main_index_file):
            self.main_index.deserialize(main_index_file)
        if os.path.exists(author_index_file):
            self.author_index.deserialize(author_index_file)
        if os.path.exists(title_index_file):
            self.title_index.deserialize(title_index_file)
        if os.path.exists(keyword_index_file):
            self.keyword_index.deserialize(keyword_index_file)
        if os.path.exists(date_index_file):
            self.date_index.deserialize(date_index_file)

    def _save_indices(self):
        self.main_index.serialize(os.path.join(
            self.index_dir, "main_index.dat"))
        self.author_index.serialize(os.path.join(
            self.index_dir, "author_index.dat"))
        self.title_index.serialize(os.path.join(
            self.index_dir, "title_index.dat"))
        self.keyword_index.serialize(os.path.join(
            self.index_dir, "keyword_index.dat"))
        self.date_index.serialize(os.path.join(
            self.index_dir, "date_index.dat"))

        with open(os.path.join(self.storage_dir, "max_article_id"), "w") as f:
            f.write(str(self.max_article_id))

    def _get_max_article_id(self) -> int:
        max_id_path = os.path.join(self.storage_dir, "max_article_id")

        if os.path.exists(max_id_path):
            with open(max_id_path, "r") as f:
                return int(f.read().strip())

        with open(max_id_path, "w") as f:
            f.write("0")

        return 0

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

    def _clear_cache(self) -> None:
        self.count_complete_subgraphs_with_progress.cache.clear()
        self.get_yearly_keyword_frequencies.cache_clear()
        self.get_author_article_counts.cache_clear()

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

        # update keyword index
        for keyword in article.keywords:
            kw_articles = self.keyword_index.find(keyword)
            if kw_articles is None:
                self.keyword_index.insert(keyword, [article.article_id])
            else:
                if article.article_id not in kw_articles:
                    kw_articles.append(article.article_id)
                    self.keyword_index.update(keyword, kw_articles)

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
            self._clear_cache()

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

    def get_collaborators(self, author: str) -> Dict[str, int]:
        articles = self.get_articles_by_author(author)
        collaborators = {}

        for article in articles:
            for coauthor in article.authors:
                if coauthor != author:
                    if coauthor in collaborators.keys():
                        collaborators[coauthor] += 1
                    else:
                        collaborators[coauthor] = 1

        return collaborators

    def get_collaborators_only(self, author: str) -> List[str]:
        articles = self.get_articles_by_author(author)
        collaborators = set()

        for article in articles:
            for coauthor in article.authors:
                if coauthor != author:
                    collaborators.add(coauthor)

        return list(collaborators)

    def get_coauthor_articles(self, author: str, coauthor: str) -> List[Article]:
        articles = self.get_articles_by_author(author)
        articles = [
            article for article in articles if coauthor in article.authors]

        return articles

    def search_articles_by_title(self, title_pattern: str) -> List[Article]:
        kws = extract_keywords_basic(title_pattern)

        result_set = set(self.keyword_index.find(kws[0]))

        for kw in kws[1:]:
            result_set.intersection_update(set(self.keyword_index.find(kw)))

        matched_articles = [self.get_article_by_id(id) for id in result_set]
        return matched_articles

    @lru_cache(maxsize=None)
    def get_author_article_counts(self) -> Dict[str, int]:
        author_counts = {}

        all_authors = self.author_index.getAllKeys()

        for author in all_authors:
            articles = self.author_index.find(author)
            if articles:
                author_counts[author] = len(articles)

        return author_counts

    @lru_cache(maxsize=None)
    def get_yearly_keyword_frequencies(self) -> Dict[int, Dict[str, float]]:
        yearly_keywords = {}
        yearly_total_articles = {}

        years = self.date_index.getAllKeys()[1:]
        article_ids_per_year = self.date_index.getAllValues()[1:]
        for year, article_ids in zip(years, article_ids_per_year):
            if year == 0:
                continue
            yearly_total_articles[year] = len(article_ids)
            yearly_keywords[year] = {}

        keyword_count = 0

        keywords = self.keyword_index.getAllKeys()
        keyword_article_ids = self.keyword_index.getAllValues()

        blacklist = ["based", "of", "the", "using"]
        filtered_data = [(word, ids) for word, ids in zip(
            keywords, keyword_article_ids) if word not in blacklist]

        article_to_year = {}
        for year, article_ids in zip(years, article_ids_per_year):
            for article_id in article_ids:
                article_to_year[article_id] = year

        for keyword, article_ids in filtered_data:
            keyword_count += 1
            for article_id in article_ids:
                year = article_to_year.get(article_id)
                if year is not None:
                    if keyword not in yearly_keywords[year]:
                        yearly_keywords[year][keyword] = 0
                    yearly_keywords[year][keyword] += 1

        freq_count = 0
        for year in yearly_keywords:
            total = yearly_total_articles[year]
            for keyword in yearly_keywords[year]:
                yearly_keywords[year][keyword] = yearly_keywords[year][keyword] / total
                freq_count += 1

        return yearly_keywords

    @cached(cache={}, key=lambda *args, **kwargs: 1)
    def count_complete_subgraphs_with_progress(self, progress_callback=None):
        adjacency_list = self.build_adjacency_list_with_progress(
            progress_callback)

        if progress_callback:
            progress_callback("pivoter", 0, 100)

        result = pivoter(adjacency_list)

        if progress_callback:
            progress_callback("pivoter", 100, 100)

        result = {key: int(value) for key, value in result.items()}

        return result

    def build_adjacency_list_with_progress(self, progress_callback=None):
        all_authors = list(self.author_index.getAllKeys())
        author_to_id = {author: idx for idx, author in enumerate(all_authors)}
        num_vertices = len(all_authors)

        adjacency_list = [set() for _ in range(num_vertices)]

        total_authors = len(all_authors)
        for idx, author in enumerate(all_authors):
            author_id = author_to_id[author]
            collaborators = self.get_collaborators_only(author)

            for collaborator in collaborators:
                if collaborator in author_to_id:
                    collaborator_id = author_to_id[collaborator]

                    adjacency_list[author_id].add(collaborator_id)
                    adjacency_list[collaborator_id].add(author_id)

            if progress_callback and (idx % 1000 == 0 or idx == total_authors - 1):
                progress_callback("build_adjacency_list",
                                  idx + 1, total_authors)

        return adjacency_list
