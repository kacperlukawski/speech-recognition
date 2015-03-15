#ifndef KMEANS_H
#define KMEANS_H

#include "IClusteringMethod.h"

#include <ostream>

namespace speech {

    namespace clustering {

        //
        // This is an implementation of KMeans algorithm.
        //
        // @see http://en.wikipedia.org/wiki/K-means_clustering
        //
        class KMeans : public IClusteringMethod {
        public:
            static const uint32_t TYPE_IDENTIFIER = 0x00000001;

            const unsigned int MAX_ITERATIONS = 100000;
            const double EPS = 10e-8;

            KMeans(std::istream& in);

            KMeans(unsigned int _k, unsigned int _dim);

            virtual ~KMeans();

            virtual void fit(std::vector<double *> vectors, std::vector<int> labels);

            virtual int predict(double *vector);

            virtual void serialize(std::ostream &out) const;
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