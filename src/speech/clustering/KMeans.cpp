#include "KMeans.h"
#include "exception/TooLessVectorsException.h"

#include <iostream>
#include <cmath>
#include <map>

speech::clustering::KMeans::KMeans(unsigned int _k, unsigned int _dim) : k(_k), dimension(_dim) {
    centroids = new std::vector<double*>();
}

speech::clustering::KMeans::~KMeans() {
    std::vector<double *>::const_iterator centroidsIt;
    for (centroidsIt = centroids->begin(); centroidsIt != centroids->end(); ++centroidsIt) {
        delete *centroidsIt;
    }

    delete centroids;
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
        throw speech::clustering::exception::TooLessVectorsException();
    }

    // @todo change a way of choosing initial centroids to be random
    unsigned int currentCentroidPosition = 0;
    std::vector<double *>::iterator vectorsIt;
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
            double *vectorCopy = new double[dimension];
            for (int i = 0; i < dimension; ++i) {
                vectorCopy[i] = vector[i];
            }

            centroids->push_back(vectorCopy);
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
        throw speech::clustering::exception::TooLessVectorsException();
    }

    for (int iteration = 0; iteration < MAX_ITERATIONS; ++iteration) {
        int *numberOfCloseVectors = new int[k];
        std::vector<double *> *closestVectorsSum = new std::vector<double *>();
        for (int i = 0; i < k; ++i) {
            double *centroidSum = new double[dimension];
            std::fill_n(centroidSum, dimension, 0.0);
            closestVectorsSum->push_back(centroidSum);

            numberOfCloseVectors[i] = 0;
        }

        for (vectorsIt = vectors.begin(); vectorsIt != vectors.end(); ++vectorsIt) {
            int currentVectorLabel = predict(*vectorsIt);

            for (int i = 0; i < dimension; ++i) {
                (closestVectorsSum->at(currentVectorLabel))[i] += (*vectorsIt)[i];
            }

            numberOfCloseVectors[currentVectorLabel]++;
        }

        double difference = 0.0;
        for (int i = 0; i < k; ++i) {
            for (int j = 0; j < dimension; ++j) {
                double normalizedCoeff = (closestVectorsSum->at(i))[j] / numberOfCloseVectors[i];

                difference += pow((centroids->at(i))[j] - normalizedCoeff, 2.0);
                (centroids->at(i))[j] = normalizedCoeff;
            }
        }

        if (difference < EPS) {
            break;
        }
    }
}

/**
 * Select the most probable label for the given vector.
 * Label is a position of the closest centroid.
 *
 * @return predicted label
 */
int speech::clustering::KMeans::predict(double *vector) {
    if (centroids->empty()) {
        // @todo probably throw an exception there, because the model was not fitted properly
        return 0;
    }

    int argmin = 0;
    int centroidsNumber = centroids->size();
    double currentMinimumDistance = distance(vector, centroids->at(0));
    for (int i = 1; i < centroidsNumber; ++i) {
        double currentDistance = distance(vector, centroids->at(i));
        if (currentDistance < currentMinimumDistance) {
            currentMinimumDistance = currentDistance;
            argmin = i;
        }
    }

    return argmin;
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

namespace speech {

    namespace clustering {

        /**
         * Load the KMeans class instance from given input stream
         *
         * @return the same stream as was given
         */
        std::istream& operator>> (std::istream& in, speech::clustering::KMeans& kMeans) {
            unsigned long centroidsNb = 0;

            in.read((char *) &kMeans.k, sizeof(unsigned int));
            in.read((char *) &kMeans.dimension, sizeof(unsigned int));
            in.read((char *) &centroidsNb, sizeof(unsigned long));

            unsigned int singleVectorSize = sizeof(double) * kMeans.dimension;

            for (int i = 0; i < centroidsNb; ++i) {
                double* currentVector = new double[kMeans.dimension];
                in.read((char *) currentVector, singleVectorSize);
                kMeans.centroids->push_back(currentVector);
            }

            return in;
        }

        /**
         * Save the instance of KMeans class into given output stream
         *
         * @return the same stream as was given
         */
        std::ostream& operator<< (std::ostream& out, const speech::clustering::KMeans& kMeans) {
            unsigned int singleVectorSize = sizeof(double) * kMeans.dimension;
            unsigned long centroidsNb = kMeans.centroids->size();

            out.write((char const *) &kMeans.k, sizeof(unsigned int));
            out.write((char const *) &kMeans.dimension, sizeof(unsigned int));
            out.write((char const *) &centroidsNb, sizeof(unsigned long));

            std::vector<double *>::const_iterator it;
            for (it = kMeans.centroids->begin(); it != kMeans.centroids->end(); ++it) {
                out.write((char const *)(*it), singleVectorSize);
            }

            return out;
        }

    }

}