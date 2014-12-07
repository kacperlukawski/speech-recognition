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
        public:
            const unsigned int MAX_ITERATIONS = 100000;
            const double EPS = 10e-8;

            KMeans(unsigned int _k, unsigned int _dim);

            virtual ~KMeans();

            virtual void fit(std::vector<double *> vectors, std::vector<int> labels);

            virtual int predict(double *vector);

            friend std::istream &operator>>(std::istream &in, speech::clustering::KMeans &kMeans);

            friend std::ostream &operator<<(std::ostream &out, const speech::clustering::KMeans &kMeans);

        protected:

        private:
            unsigned int k; // number of clusters
            unsigned int dimension; // single vector dimension
            std::vector<double *> *centroids;

            inline double distance(double *v1, double *v2);
        };

    }

}

#endif