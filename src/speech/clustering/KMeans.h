#ifndef KMEANS_H
#define KMEANS_H

#include "IClusteringMethod.h"

#include "../metric/IMetric.h"
#include "../metric/EuclideanDistance.h"
#include "../metric/MaximumDistance.h"

#include <ostream>

namespace speech {

    namespace clustering {

        /**
         * This is an implementation of KMeans algorithm.
         * @see http://en.wikipedia.org/wiki/K-means_clustering
         */
        class KMeans : public IClusteringMethod {
        public:
            static const uint32_t TYPE_IDENTIFIER = 0x00000001;
            const unsigned int MAX_ITERATIONS = 100000;
            const double EPS = 10e-64;

            /**
             * Construct a clustering method from a stream.
             * @param in stream containing serialized model
             */
            KMeans(std::istream &in);

            /**
             * Construct a clustering method with given parameters.
             * @param _k number of clusters
             * @param _dim dimension of a single vector
             */
            KMeans(unsigned int _k, unsigned int _dim);

            /**
             * Destructor
             */
            virtual ~KMeans();

            /**
             * Fit the KMeans model using standard algorithm:
             * 1. Randomly choose k centroids from given vectors' set
             * 2. In each iteration assign all vectors into the nearest centroid
             * 3. Update each centroid to be a mean of the all vectors belonging to this particular group
             * 4. Stop when nothing changed in an iteration or after maximum number of iterations
             */
            virtual void fit(vector<valarray<double>> &vectors);

            /**
             * Select the most probable label for the given vector.
             * Label is a position of the closest centroid.
             *
             * @return predicted label
             */
            virtual int predict(const valarray<double> &vector);

            inline std::vector<std::valarray<double>> *getCentroids() const {
                return this->centroids;
            }

            /**
             * Serialize a model into given stream.
             * @param out stream to serialize the model into
             */
            virtual void serialize(std::ostream &out) const;

        protected:
//            void chooseCentroids(vector<valarray<double>> &vectors);

        private:
            /** Number of clusters */
            unsigned int k;
            /** Single vector dimension */
            unsigned int dimension;
            /** List of centroids */
            std::vector<valarray<double>> *centroids;
            /** Distance function */
            speech::metric::IMetric* metric = new speech::metric::MaximumDistance();
        };

    }

}

#endif