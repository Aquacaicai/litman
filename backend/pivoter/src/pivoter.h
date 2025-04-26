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

#ifndef PIVOTER_H
#define PIVOTER_H

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <limits>
#include <algorithm>
#include <gmp.h>

#include "misc.h"
#include "neighbor_list.h"

void listAllCliquesDegeneracyRecursive_A(mpz_t* cliqueCounts,
    int* vertexSets, int* vertexLookup,
    int** neighborsInP, int* numNeighbors,
    int beginX, int beginP, int beginR, int max_k,
    int rsize, int drop);

void listAllCliquesDegeneracy_A(mpz_t* cliqueCounts, NeighborListArray** orderingArray,
    int size, int max_k) {

    // vertex sets are stored in an array like this: |--X--|--P--|
    int* vertexSets = new int[size]();

    // vertex i is stored in vertexSets[vertexLookup[i]]
    int* vertexLookup = new int[size]();

    int** neighborsInP = new int* [size];
    int* numNeighbors = new int[size]();

    int i = 0;

    while (i < size) {
        vertexLookup[i] = i;
        vertexSets[i] = i;
        neighborsInP[i] = new int[1]();
        numNeighbors[i] = 1;
        i++;
    }

    int beginX = 0;
    int beginP = 0;
    int beginR = size;

    // for each vertex
    for (i = 0; i < size; i++) {
        int vertex = orderingArray[i]->vertex;

        int newBeginX, newBeginP, newBeginR;

        // set P to be later neighbors and X to be be earlier neighbors of vertex
        fillInPandXForRecursiveCallDegeneracyCliques(i, vertex,
            vertexSets, vertexLookup,
            orderingArray,
            neighborsInP, numNeighbors,
            &beginX, &beginP, &beginR,
            &newBeginX, &newBeginP, &newBeginR);


        // recursively compute maximal cliques containing vertex, some of its
        // later neighbors, and avoiding earlier neighbors
        int drop = 0;
        int rsize = 1;

        listAllCliquesDegeneracyRecursive_A(cliqueCounts,
            vertexSets, vertexLookup,
            neighborsInP, numNeighbors,
            newBeginX, newBeginP, newBeginR, max_k, rsize, drop);

        beginR = beginR + 1;
    }

    mpz_set_ui(cliqueCounts[0], 1);

    delete[] vertexSets;
    delete[] vertexLookup;

    for (i = 0; i < size; i++) {
        delete[] neighborsInP[i];
        delete[] orderingArray[i]->later;
        delete[] orderingArray[i]->earlier;
        delete orderingArray[i];
    }

    delete[] orderingArray;
    delete[] neighborsInP;
    delete[] numNeighbors;

    return;
}

void listAllCliquesDegeneracyRecursive_A(mpz_t* cliqueCounts,
    int* vertexSets, int* vertexLookup,
    int** neighborsInP, int* numNeighbors,
    int beginX, int beginP, int beginR, int max_k,
    int rsize, int drop) {

    if ((beginP >= beginR) || (rsize - drop > max_k)) {
        for (int i = drop; (i >= 0) && (rsize - i <= max_k); i--) {
            int k = rsize - i;

            mpz_t temp, n_mpz, i_mpz;
            mpz_init(temp);
            mpz_init(n_mpz);
            mpz_init(i_mpz);

            mpz_set_ui(n_mpz, drop);
            mpz_set_ui(i_mpz, i);

            mpz_bin_ui(temp, n_mpz, i);

            mpz_add(cliqueCounts[k], cliqueCounts[k], temp);

            mpz_clear(temp);
            mpz_clear(n_mpz);
            mpz_clear(i_mpz);
        }

        return;
    }

    int* myCandidatesToIterateThrough;
    int numCandidatesToIterateThrough = 0;

    // get the candidates to add to R to make a maximal clique
    int pivot = findBestPivotNonNeighborsDegeneracyCliques(&myCandidatesToIterateThrough,
        &numCandidatesToIterateThrough,
        vertexSets, vertexLookup,
        neighborsInP, numNeighbors,
        beginX, beginP, beginR);

    // add candiate vertices to the partial clique one at a time and 
    // search for maximal cliques
    if (numCandidatesToIterateThrough != 0) {
        int iterator = 0;
        while (iterator < numCandidatesToIterateThrough) {
            // vertex to be added to the partial clique
            int vertex = myCandidatesToIterateThrough[iterator];

            int newBeginX, newBeginP, newBeginR;

            // add vertex into partialClique, representing R.

            // swap vertex into R and update all data structures 
            moveToRDegeneracyCliques(vertex,
                vertexSets, vertexLookup,
                neighborsInP, numNeighbors,
                &beginX, &beginP, &beginR,
                &newBeginX, &newBeginP, &newBeginR);

            // recursively compute maximal cliques with new sets R, P and X
            if (vertex == pivot)
                listAllCliquesDegeneracyRecursive_A(cliqueCounts,
                    vertexSets, vertexLookup,
                    neighborsInP, numNeighbors,
                    newBeginX, newBeginP, newBeginR, max_k, rsize + 1, drop + 1);
            else
                listAllCliquesDegeneracyRecursive_A(cliqueCounts,
                    vertexSets, vertexLookup,
                    neighborsInP, numNeighbors,
                    newBeginX, newBeginP, newBeginR, max_k, rsize + 1, drop);

            moveFromRToXDegeneracyCliques(vertex,
                vertexSets, vertexLookup,
                &beginX, &beginP, &beginR);

            iterator++;
        }

        // swap vertices that were moved to X back into P, for higher recursive calls.
        iterator = 0;
        while (iterator < numCandidatesToIterateThrough) {
            int vertex = myCandidatesToIterateThrough[iterator];
            int vertexLocation = vertexLookup[vertex];

            beginP--;
            vertexSets[vertexLocation] = vertexSets[beginP];
            vertexSets[beginP] = vertex;
            vertexLookup[vertex] = beginP;
            vertexLookup[vertexSets[vertexLocation]] = vertexLocation;

            iterator++;
        }
    }

    // don't need to check for emptiness before freeing, since
    // something will always be there (we allocated enough memory
    // for all of P, which is nonempty)
    delete[] myCandidatesToIterateThrough;

    return;
}

#endif // PIVOTER_H
