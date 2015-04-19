#include "KMeans.h"
#include "exception/TooLessVectorsException.h"

#include <iostream>
#include <cmath>
#include <map>

speech::clustering::KMeans::KMeans(std::istream &in) {
    unsigned long centroidsNb = 0;

    in.read((char *) &k, sizeof(k));
    in.read((char *) &dimension, sizeof(dimension));
    in.read((char *) &centroidsNb, sizeof(centroidsNb));

    unsigned int singleVectorSize = sizeof(double) * dimension;

    centroids = new std::vector<double*>(centroidsNb);
    for (int i = 0; i < centroidsNb; i++) {
        double * vector = new double[dimension];
        in.read((char *) vector, singleVectorSize);
        (*centroids)[i] = vector;
    }
}

speech::clustering::KMeans::KMeans(unsigned int _k, unsigned int _dim)
        : k(_k), dimension(_dim) {
    centroids = new std::vector<double*>();
}

speech::clustering::KMeans::~KMeans() {
    std::vector<double *>::const_iterator centroidsIt;
    for (centroidsIt = centroids->begin(); centroidsIt != centroids->end(); ++centroidsIt) {
        delete *centroidsIt;
    }

    delete centroids;
}

#include <iostream> // TODO: remove this entry

//
// Fit the KMeans model using standard algorithm:
// 1. Randomly choose k centroids from given vectors' set
// 2. In each iteration assign all vectors into the nearest centroid
// 3. Update each centroid to be a mean of the all vectors belonging to this particular group
// 4. Stop when nothing changed in an iteration or after maximum number of iterations
//
void speech::clustering::KMeans::fit(std::vector<double *> &vectors, std::vector<int> &labels) {
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

    std::cout << "Centroids: " << std::endl;
    for (centroidsIt = centroids->begin(); centroidsIt != centroids->end(); ++centroidsIt) {
        std::cout << "[ ";
        for (int i = 0; i < dimension; i++) {
            std::cout << (*centroidsIt)[i] << " ";
        }
        std::cout << "]" << std::endl;
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

//
// Save the instance of KMeans class into given output stream
//
void speech::clustering::KMeans::serialize(std::ostream &out) const {
    uint32_t typeIdentifier = TYPE_IDENTIFIER;
    out.write((char const *) &typeIdentifier, sizeof(typeIdentifier));

    unsigned int singleVectorSize = sizeof(double) * dimension;
    unsigned long centroidsNb = centroids->size();

    out.write((char const *) &k, sizeof(unsigned int));
    out.write((char const *) &dimension, sizeof(unsigned int));
    out.write((char const *) &centroidsNb, sizeof(unsigned long));

    std::vector<double *>::const_iterator it;
    for (it = centroids->begin(); it != centroids->end(); ++it) {
        out.write((char const *)(*it), singleVectorSize);
    }
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