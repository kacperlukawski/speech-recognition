#include "KMeans.h"

speech::clustering::KMeans::KMeans(int _k, int _dim) : k(_k), dimension(_dim) {
    centroids = new double*[k];
    for (int i = 0; i < k; ++i) {
        centroids[i] = new double[dimension];
    }
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
void speech::clustering::KMeans::fit(std::list<double *> vectors, int *labels) {

}

/**
 * @todo select the most probable label for the given vector
 */
int speech::clustering::KMeans::predict(double *vector) {
    return 0;
}

/**
 * @todo calculate Euclidean distance of two given vectors
 */
double speech::clustering::KMeans::distance(double *v1, double *v2) {
    return 0.0;
}