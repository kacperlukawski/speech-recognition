#include "KMeans.h"

#include <cmath>

speech::clustering::KMeans::KMeans(int _k, int _dim) : k(_k), dimension(_dim) {
    centroids = new std::vector<double*>();
}

speech::clustering::KMeans::~KMeans() {
    delete[] centroids;
}

/**
 * @todo prepare an implementation of the K-Means
 *
 * 1. randomly choose k centroids from given vectors' set
 * 2. in each iteration assign all vectors into the nearest centroid
 * 3. update each centroid to be a mean of the all vectors belonging to this particular group
 * 4. stop when nothing changed in an iteration or after maximum number of iterations
 */
void speech::clustering::KMeans::fit(std::vector<double *> vectors, std::vector<int> labels) {
    int vectorsNumber = vectors.size();
    if (vectorsNumber < k) {
        // current dataset is not enough to create K clusters
        // @todo throw an exception
        return;
    }

    // @todo change a way of choosing initial centroids to be random
    unsigned int currentCentroidPosition = 0;
    std::vector<double *>::const_iterator vectorsIt;
    std::vector<double *>::const_iterator centroidsIt;
    for (vectorsIt = vectors.begin(); vectorsIt != vectors.end(); ++vectorsIt) {
        double* vector = *vectorsIt;

        bool addCurrentVector = true;
        for (centroidsIt = centroids->begin(); centroidsIt != centroids->end(); ++centroidsIt) {
            if (distance(vector, *centroidsIt) == 0.0) {
                addCurrentVector = false;
                break;
            }
        }

        if (addCurrentVector) {
            centroids->push_back(vector);
            currentCentroidPosition++;
        }

        if (currentCentroidPosition == k) {
            break;
        }
    }

    if (currentCentroidPosition < k) {
        // number of centroids is not enough
        // probably the initial collection of vectors
        // contains many duplicates
        // @todo throw an exception
    }


}

/**
 * @todo select the most probable label for the given vector
 */
int speech::clustering::KMeans::predict(double *vector) {
    if (centroids->empty()) {
        // @todo probably throw an exception there, because the model was not fitted properly
        return 0;
    }


    std::vector<double *>::const_iterator centroidsIt;
    for (centroidsIt = centroids->begin(); centroidsIt != centroids->end(); ++centroidsIt) {

    }

    return 0;
}

/**
 * Calculate Euclidean distance of two given vectors
 *
 * @return distance between vectors
 */
double speech::clustering::KMeans::distance(double *v1, double *v2) {
    double sum = 0.0;
    for (int i = 0; i < dimension; ++i) {
        sum += pow(v1[i] - v2[i], 2.0);
    }

    return sqrt(sum);
}