/*
    A C++ implement of Pivoter algorithm in "The power of pivoting for
    exact clique counting." (WSDM 2020).

    Copyright (C) 2011  Darren Strash
    Copyright (C) 2020  Shweta Jain
    Copyright (C) 2025  ParaN3xus

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

    Contact ParaN3xus by: paran3xus007@gmail.com
*/

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


#endif // MISC_H
