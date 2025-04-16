from typing import List, Dict, Tuple, Set
from backend.models.storage import LiteratureStorage


class StatsService:
    def __init__(self, storage: LiteratureStorage):
        self.storage = storage

    def get_author_article_counts(self) -> List[Tuple[str, int]]:
        counts = self.storage.get_author_article_counts()
        return sorted(counts.items(), key=lambda x: x[1], reverse=True)

    def get_yearly_keyword_frequencies(self) -> Dict[int, List[Tuple[str, int]]]:
        yearly_keywords = self.storage.get_yearly_keyword_frequencies()

        result = {}
        for year, keywords in yearly_keywords.items():
            result[year] = sorted(
                keywords.items(), key=lambda x: x[1], reverse=True)

        return result

    def get_collaboration_network(self) -> Dict[str, Set[str]]:
        return self.storage.get_collaboration_graph()

    def count_complete_subgraphs(self) -> Dict[int, int]:
        graph = self.storage.get_collaboration_graph()
        return self.storage.count_complete_subgraphs(graph)
