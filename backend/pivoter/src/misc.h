#ifndef MISC_H
#define MISC_H

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cassert>
#include <string>
#include <algorithm>
#include <gmp.h>

#include "neighbor_list.h"


void moveFromRToXDegeneracyCliques(int vertex,
    int* vertexSets, int* vertexLookup,
    int* pBeginX, int* pBeginP, int* pBeginR);

void moveToRDegeneracyCliques(int vertex,
    int* vertexSets, int* vertexLookup,
    int** neighborsInP, int* numNeighbors,
    int* pBeginX, int* pBeginP, int* pBeginR,
    int* pNewBeginX, int* pNewBeginP, int* pNewBeginR);

void fillInPandXForRecursiveCallDegeneracyCliques(int vertex, int orderNumber,
    int* vertexSets, int* vertexLookup,
    NeighborListArray** orderingArray,
    int** neighborsInP, int* numNeighbors,
    int* pBeginX, int* pBeginP, int* pBeginR,
    int* pNewBeginX, int* pNewBeginP, int* pNewBeginR);

int findBestPivotNonNeighborsDegeneracyCliques(int** pivotNonNeighbors, int* numNonNeighbors,
    int* vertexSets, int* vertexLookup,
    int** neighborsInP, int* numNeighbors,
    int beginX, int beginP, int beginR);

// Implementation


int findBestPivotNonNeighborsDegeneracyCliques(int** pivotNonNeighbors, int* numNonNeighbors,
    int* vertexSets, int* vertexLookup,
    int** neighborsInP, int* numNeighbors,
    int beginX, int beginP, int beginR) {
    int pivot = -1;
    int maxIntersectionSize = -1;

    // iterate over each vertex in P union X 
    // to find the vertex with the most neighbors in P.
    int j = beginP;
    while (j < beginR) {
        int vertex = vertexSets[j];
        int numPotentialNeighbors = std::min(beginR - beginP, numNeighbors[vertex]);

        int numNeighborsInP = 0;

        int k = 0;
        while (k < numPotentialNeighbors) {
            int neighbor = neighborsInP[vertex][k];
            int neighborLocation = vertexLookup[neighbor];

            if (neighborLocation >= beginP && neighborLocation < beginR) {
                numNeighborsInP++;
            }
            else {
                break;
            }

            k++;
        }

        if (numNeighborsInP > maxIntersectionSize) {
            pivot = vertex;
            maxIntersectionSize = numNeighborsInP;
        }

        j++;
    }

    // compute non neighbors of pivot by marking its neighbors
    // and moving non-marked vertices into pivotNonNeighbors.
    *pivotNonNeighbors = new int[beginR - beginP]();
    memcpy(*pivotNonNeighbors, &vertexSets[beginP], (beginR - beginP) * sizeof(int));

    // we will decrement numNonNeighbors as we find neighbors
    *numNonNeighbors = beginR - beginP;

    int numPivotNeighbors = std::min(beginR - beginP, numNeighbors[pivot]);

    // mark the neighbors of pivot that are in P.
    j = 0;
    while (j < numPivotNeighbors) {
        int neighbor = neighborsInP[pivot][j];
        int neighborLocation = vertexLookup[neighbor];

        if (neighborLocation >= beginP && neighborLocation < beginR) {
            (*pivotNonNeighbors)[neighborLocation - beginP] = -1;
        }
        else {
            break;
        }

        j++;
    }

    // move non-neighbors of pivot in P to the beginning of
    // pivotNonNeighbors and set numNonNeighbors appriopriately.
    j = 0;
    while (j < *numNonNeighbors) {
        int vertex = (*pivotNonNeighbors)[j];

        if (vertex == -1) {
            (*numNonNeighbors)--;
            (*pivotNonNeighbors)[j] = (*pivotNonNeighbors)[*numNonNeighbors];
            continue;
        }

        j++;
    }

    return pivot;
}

void fillInPandXForRecursiveCallDegeneracyCliques(int vertex, int orderNumber,
    int* vertexSets, int* vertexLookup,
    NeighborListArray** orderingArray,
    int** neighborsInP, int* numNeighbors,
    int* pBeginX, int* pBeginP, int* pBeginR,
    int* pNewBeginX, int* pNewBeginP, int* pNewBeginR) {
    int vertexLocation = vertexLookup[vertex];

    (*pBeginR)--;
    vertexSets[vertexLocation] = vertexSets[*pBeginR];
    vertexLookup[vertexSets[*pBeginR]] = vertexLocation;
    vertexSets[*pBeginR] = vertex;
    vertexLookup[vertex] = *pBeginR;

    *pNewBeginR = *pBeginR;
    *pNewBeginP = *pBeginR;

    // swap later neighbors of vertex into P section of vertexSets
    int j = 0;
    while (j < orderingArray[orderNumber]->laterDegree) {
        int neighbor = orderingArray[orderNumber]->later[j];
        int neighborLocation = vertexLookup[neighbor];

        (*pNewBeginP)--;

        vertexSets[neighborLocation] = vertexSets[*pNewBeginP];
        vertexLookup[vertexSets[*pNewBeginP]] = neighborLocation;
        vertexSets[*pNewBeginP] = neighbor;
        vertexLookup[neighbor] = *pNewBeginP;

        j++;
    }

    *pNewBeginX = *pNewBeginP;

    // reset numNeighbors and neighborsInP for this vertex
    j = *pNewBeginP;
    while (j < *pNewBeginR) {
        int vertexInP = vertexSets[j];
        numNeighbors[vertexInP] = 0;
        delete[] neighborsInP[vertexInP];
        neighborsInP[vertexInP] = new int[std::min(*pNewBeginR - *pNewBeginP,
            orderingArray[vertexInP]->laterDegree
            + orderingArray[vertexInP]->earlierDegree)]();

        j++;
    }

    // count neighbors in P, and fill in array of neighbors
    // in P
    j = *pNewBeginP;
    while (j < *pNewBeginR) {
        int vertexInP = vertexSets[j];

        int k = 0;
        while (k < orderingArray[vertexInP]->laterDegree) {
            int laterNeighbor = orderingArray[vertexInP]->later[k];
            int laterNeighborLocation = vertexLookup[laterNeighbor];

            if (laterNeighborLocation >= *pNewBeginP && laterNeighborLocation < *pNewBeginR) {
                neighborsInP[vertexInP][numNeighbors[vertexInP]] = laterNeighbor;
                numNeighbors[vertexInP]++;
                neighborsInP[laterNeighbor][numNeighbors[laterNeighbor]] = vertexInP;
                numNeighbors[laterNeighbor]++;
            }

            k++;
        }

        j++;
    }
}

void moveToRDegeneracyCliques(int vertex,
    int* vertexSets, int* vertexLookup,
    int** neighborsInP, int* numNeighbors,
    int* pBeginX, int* pBeginP, int* pBeginR,
    int* pNewBeginX, int* pNewBeginP, int* pNewBeginR) {

    int vertexLocation = vertexLookup[vertex];

    (*pBeginR)--;
    vertexSets[vertexLocation] = vertexSets[*pBeginR];
    vertexLookup[vertexSets[*pBeginR]] = vertexLocation;
    vertexSets[*pBeginR] = vertex;
    vertexLookup[vertex] = *pBeginR;

    // this is not a typo, initially newX is empty
    *pNewBeginX = *pBeginP;
    *pNewBeginP = *pBeginP;
    *pNewBeginR = *pBeginP;

    int sizeOfP = *pBeginR - *pBeginP;

    int j = (*pBeginP);
    while (j < (*pBeginR)) {
        int neighbor = vertexSets[j];
        int neighborLocation = j;

        int numPotentialNeighbors = std::min(sizeOfP, numNeighbors[neighbor]);
        int k = 0;
        while (k < numPotentialNeighbors) {
            if (neighborsInP[neighbor][k] == vertex) {
                vertexSets[neighborLocation] = vertexSets[(*pNewBeginR)];
                vertexLookup[vertexSets[(*pNewBeginR)]] = neighborLocation;
                vertexSets[(*pNewBeginR)] = neighbor;
                vertexLookup[neighbor] = (*pNewBeginR);
                (*pNewBeginR)++;
            }

            k++;
        }

        j++;
    }

    j = (*pNewBeginP);

    while (j < *pNewBeginR) {
        int thisVertex = vertexSets[j];

        int numPotentialNeighbors = std::min(sizeOfP, numNeighbors[thisVertex]);

        int numNeighborsInP = 0;

        int k = 0;
        while (k < numPotentialNeighbors) {
            int neighbor = neighborsInP[thisVertex][k];
            int neighborLocation = vertexLookup[neighbor];
            if (neighborLocation >= *pNewBeginP && neighborLocation < *pNewBeginR) {
                neighborsInP[thisVertex][k] = neighborsInP[thisVertex][numNeighborsInP];
                neighborsInP[thisVertex][numNeighborsInP] = neighbor;
                numNeighborsInP++;
            }
            k++;
        }

        j++;
    }
}

void moveFromRToXDegeneracyCliques(int vertex,
    int* vertexSets, int* vertexLookup,
    int* pBeginX, int* pBeginP, int* pBeginR) {
    int vertexLocation = vertexLookup[vertex];

    //swap vertex into X and increment beginP and beginR
    vertexSets[vertexLocation] = vertexSets[*pBeginP];
    vertexLookup[vertexSets[*pBeginP]] = vertexLocation;
    vertexSets[*pBeginP] = vertex;
    vertexLookup[vertex] = *pBeginP;

    *pBeginP = *pBeginP + 1;
    *pBeginR = *pBeginR + 1;
}



std::list<int>** readInGraphAdjListToDoubleEdges(int* n, int* m, const char* fpath) {
    int u, v; // endvertices, to read edges.

    FILE* fp;
    fp = fopen(fpath, "r");
    if (!fp) {
        fprintf(stderr, "Could not open input file.\n");
        exit(1);
    }

    if (fscanf(fp, "%d %d", n, m) != 2) {
        fprintf(stderr, "Number of vertices: %d\n", *n);
        fprintf(stderr, "Number of edges: %d\n", *m);
        fprintf(stderr, "problem with line 1 in input file\n");
        exit(1);
    }

    std::list<int>** adjList = new std::list<int>*[*n];

    int i = 0;
    while (i < *n) {
        adjList[i] = new std::list<int>();
        i++;
    }

    i = 0;
    while (i < *m) {
        if (fscanf(fp, "%d %d\n", &u, &v) != 2) {
            printf("problem with line %d in input file, u=%d, v=%d\n", i + 2, u, v);
            exit(1);
        }

        assert(u < *n && u > -1);
        assert(v < *n && v > -1);
        assert(u != v);

        adjList[u]->push_back(v);
        adjList[v]->push_back(u);

        i++;
    }

    *m = (*m) * 2;

    fclose(fp);
    return adjList;
}


#endif // MISC_H
