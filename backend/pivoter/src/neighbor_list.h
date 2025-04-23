#ifndef NEIGHTBOR_LIST_H
#define NEIGHTBOR_LIST_H

#include <vector>
#include <list>
#include <cstddef>
#include <algorithm>
#include <cstdlib>

struct NeighborList {
    int vertex; // the vertex that owns this neighbor list
    std::list<int>* earlier; // a linked list of neighbors that come before this vertex in the ordering
    std::list<int>* later; // a linked list of neighbors that come after this vertex in the ordering
    int orderNumber; // the position of this vertex in the ordering
};

struct NeighborListArray {
    int vertex; // the vertex that owns this neighbor list
    int* earlier; // an array of neighbors that come before this vertex in an ordering
    int earlierDegree; // the number of neighbors in earlier
    int* later; // an array of neighbors that come after this vertex in an ordering
    int laterDegree; // an array of neighbors that come after this vertex in an ordering
    int orderNumber; // the position of this vertex in the ordering
};

class DegeneracyOrdering {
public:
    // Compute the degeneracy of the input graph
    static int computeDegeneracy(std::list<int>** list, int size) {
        int degeneracy = 0;
        std::list<int>** verticesByDegree = new std::list<int>*[size];
        std::list<int>::iterator* vertexLocator = new std::list<int>::iterator[size];
        int* degree = new int[size]();

        for (int i = 0; i < size; i++) {
            verticesByDegree[i] = new std::list<int>();
        }

        for (int i = 0; i < size; i++) {
            degree[i] = list[i]->size();
            verticesByDegree[degree[i]]->push_front(i);
            vertexLocator[i] = verticesByDegree[degree[i]]->begin();
        }

        int currentDegree = 0;
        int numVerticesRemoved = 0;

        while (numVerticesRemoved < size) {
            if (!verticesByDegree[currentDegree]->empty()) {
                degeneracy = std::max(degeneracy, currentDegree);
                int vertex = verticesByDegree[currentDegree]->front();
                verticesByDegree[currentDegree]->erase(vertexLocator[vertex]);
                degree[vertex] = -1;

                for (int neighbor : *list[vertex]) {
                    if (degree[neighbor] != -1) {
                        verticesByDegree[degree[neighbor]]->erase(vertexLocator[neighbor]);
                        degree[neighbor]--;
                        if (degree[neighbor] != -1) {
                            verticesByDegree[degree[neighbor]]->push_front(neighbor);
                            vertexLocator[neighbor] = verticesByDegree[degree[neighbor]]->begin();
                        }
                    }
                }

                numVerticesRemoved++;
                currentDegree = 0;
            }
            else {
                currentDegree++;
            }
        }

        for (int i = 0; i < size; i++) {
            delete verticesByDegree[i];
        }
        delete[] vertexLocator;
        delete[] verticesByDegree;
        delete[] degree;

        return degeneracy;
    }

    // Compute degeneracy ordering and return as array of NeighborLists
    static NeighborList** computeDegeneracyOrderList(std::list<int>** list, int size) {
        NeighborList** ordering = new NeighborList * [size];
        std::list<int>** verticesByDegree = new std::list<int>*[size];
        std::list<int>::iterator* vertexLocator = new std::list<int>::iterator[size];
        int* degree = new int[size]();

        for (int i = 0; i < size; i++) {
            verticesByDegree[i] = new std::list<int>();
            ordering[i] = new NeighborList();
            ordering[i]->earlier = new std::list<int>();
            ordering[i]->later = new std::list<int>();
        }

        for (int i = 0; i < size; i++) {
            degree[i] = list[i]->size();
            verticesByDegree[degree[i]]->push_front(i);
            vertexLocator[i] = verticesByDegree[degree[i]]->begin();
        }

        int currentDegree = 0;
        int numVerticesRemoved = 0;

        while (numVerticesRemoved < size) {
            if (!verticesByDegree[currentDegree]->empty()) {
                int vertex = verticesByDegree[currentDegree]->front();
                verticesByDegree[currentDegree]->erase(vertexLocator[vertex]);

                ordering[vertex]->vertex = vertex;
                ordering[vertex]->orderNumber = numVerticesRemoved;
                degree[vertex] = -1;

                for (int neighbor : *list[vertex]) {
                    if (degree[neighbor] != -1) {
                        verticesByDegree[degree[neighbor]]->erase(vertexLocator[neighbor]);
                        ordering[vertex]->later->push_back(neighbor);
                        degree[neighbor]--;

                        if (degree[neighbor] != -1) {
                            verticesByDegree[degree[neighbor]]->push_front(neighbor);
                            vertexLocator[neighbor] = verticesByDegree[degree[neighbor]]->begin();
                        }
                    }
                    else {
                        ordering[vertex]->earlier->push_back(neighbor);
                    }
                }

                numVerticesRemoved++;
                currentDegree = 0;
            }
            else {
                currentDegree++;
            }
        }

        for (int i = 0; i < size; i++) {
            delete verticesByDegree[i];
        }
        delete[] vertexLocator;
        delete[] verticesByDegree;
        delete[] degree;

        return ordering;
    }


};


// Compute degeneracy ordering and return as array of NeighborListArrays
NeighborListArray** computeDegeneracyOrderArray(std::list<int>** list, int size) {
    NeighborList** ordering = new NeighborList * [size];
    std::list<int>** verticesByDegree = new std::list<int>*[size];
    std::list<int>::iterator* vertexLocator = new std::list<int>::iterator[size];
    int* degree = new int[size]();

    for (int i = 0; i < size; i++) {
        verticesByDegree[i] = new std::list<int>();
        ordering[i] = new NeighborList();
        ordering[i]->earlier = new std::list<int>();
        ordering[i]->later = new std::list<int>();
    }

    for (int i = 0; i < size; i++) {
        degree[i] = list[i]->size();
        verticesByDegree[degree[i]]->push_front(i);
        vertexLocator[i] = verticesByDegree[degree[i]]->begin();
    }

    int currentDegree = 0;
    int numVerticesRemoved = 0;

    while (numVerticesRemoved < size) {
        if (!verticesByDegree[currentDegree]->empty()) {
            int vertex = verticesByDegree[currentDegree]->front();
            verticesByDegree[currentDegree]->erase(vertexLocator[vertex]);

            ordering[vertex]->vertex = vertex;
            ordering[vertex]->orderNumber = numVerticesRemoved;
            degree[vertex] = -1;

            for (int neighbor : *list[vertex]) {
                if (degree[neighbor] != -1) {
                    verticesByDegree[degree[neighbor]]->erase(vertexLocator[neighbor]);
                    ordering[vertex]->later->push_back(neighbor);
                    degree[neighbor]--;

                    if (degree[neighbor] != -1) {
                        verticesByDegree[degree[neighbor]]->push_front(neighbor);
                        vertexLocator[neighbor] = verticesByDegree[degree[neighbor]]->begin();
                    }
                }
                else {
                    ordering[vertex]->earlier->push_back(neighbor);
                }
            }

            numVerticesRemoved++;
            currentDegree = 0;
        }
        else {
            currentDegree++;
        }
    }

    NeighborListArray** orderingArray = new NeighborListArray * [size];

    for (int i = 0; i < size; i++) {
        orderingArray[i] = new NeighborListArray();
        orderingArray[i]->vertex = ordering[i]->vertex;
        orderingArray[i]->orderNumber = ordering[i]->orderNumber;

        orderingArray[i]->laterDegree = ordering[i]->later->size();
        orderingArray[i]->later = new int[orderingArray[i]->laterDegree];

        int j = 0;
        for (int neighbor : *ordering[i]->later) {
            orderingArray[i]->later[j++] = neighbor;
        }

        orderingArray[i]->earlierDegree = ordering[i]->earlier->size();
        orderingArray[i]->earlier = new int[orderingArray[i]->earlierDegree];

        j = 0;
        for (int neighbor : *ordering[i]->earlier) {
            orderingArray[i]->earlier[j++] = neighbor;
        }
    }

    for (int i = 0; i < size; i++) {
        delete ordering[i]->earlier;
        delete ordering[i]->later;
        delete ordering[i];
        delete verticesByDegree[i];
    }

    delete[] ordering;
    delete[] vertexLocator;
    delete[] verticesByDegree;
    delete[] degree;

    return orderingArray;
}

#endif