#ifndef KMEANS_H
#define KMEANS_H

#include "IClusteringMethod.h"

#include <ostream>

namespace speech {

    namespace clustering {

        /**
         * This is an implementation of KMeans algorithm.
         *
         * @see http://en.wikipedia.org/wiki/K-means_clustering
         */
        class KMeans : public IClusteringMethod {
        private:
            const unsigned int MAX_ITERATIONS = 100000;
            const double EPS = 10e-8;

            int k; // number of clusters
            int dimension; // single vector dimension
            std::vector<double *>* centroids;

            inline double distance(double *v1, double *v2);
        public:
            KMeans(int _k, int _dim);

            ~KMeans();

            virtual void fit(std::vector<double *> vectors, std::vector<int> labels);

            virtual int predict(double *vector);

            friend std::ostream& operator<< (std::ostream&, const speech::clustering::KMeans&);
        };

    }

}

#endif