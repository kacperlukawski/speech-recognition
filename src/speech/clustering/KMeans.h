#ifndef KMEANS_H
#define KMEANS_H

#include "IClusteringMethod.h"

namespace speech {

    namespace clustering {

        /**
         * This is an implementation of KMeans algorithm.
         *
         * @see http://en.wikipedia.org/wiki/K-means_clustering
         */
        class KMeans : public IClusteringMethod {
        private:
            int k; // number of clusters
            int dimension; // single vector dimension
            double **centroids;

            inline double distance(double *v1, double *v2);
        public:
            KMeans(int _k, int _dim);

            virtual void fit(double **vectors, int *labels);

            virtual int predict(double *vector);
        };

    }

}

#endif