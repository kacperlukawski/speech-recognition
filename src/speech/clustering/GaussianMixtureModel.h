#ifndef SPEECH_RECOGNITION_GAUSIANMIXTUREMODEL_H
#define SPEECH_RECOGNITION_GAUSIANMIXTUREMODEL_H

#include "IClusteringMethod.h"

#include <istream>

extern "C" {
#include "../../../lib/vl/generic.h"
#include "../../../lib/vl/gmm.h"
}

namespace speech {

    namespace clustering {

        /**
         * Gaussian mixture model (GMM) implementation
         */
        class GaussianMixtureModel : public IClusteringMethod {
        public:

            static const uint32_t TYPE_IDENTIFIER = 0x00000002;

            /**
             * Creates a Gaussian mixture model from the stream which contains
             * serialized model. All parameters are loaded into the model, which
             * can be then taught further.
             * @param in input stream
             */
            GaussianMixtureModel(std::istream& in);

            /**
             * Creates a Gaussian mixture model for given number of clusters and
             * dimension of a single data vector.
             * @param nbClusters number of clusters
             * @param dimension single data vector dimension
             */
            GaussianMixtureModel(int nbClusters, int dimension);

            /**
             * Destructor
             */
            ~GaussianMixtureModel();

            virtual void fit(vector<valarray<double>> &vectors);

            virtual int predict(const valarray<double> &vector);

            virtual void serialize(std::ostream &out) const;

        private:
            /** Number of clusters */
            int nbClusters;

            /** Dimension of one single data vector */
            int dimension;

            /** VLFeat GMM instance */
            VlGMM *gmm;

            /** Means of the probability distribution - number of clusters X single vector dimension */
            double *means;

            /** Convariances matrix - number of clusters X single vector dimension */
            double *covariances;

            /** Prior probabilities for each cluster */
            double *priors;

            /** Posterior probabilities for each cluster */
            double *posteriors;

            /** Log likelihood obtained by GMM training */
            double logLikelihood;

            /**
             * Creates a representation of the data used by VLFeat library
             * @param vectors collection of the input data vectors
             *
             * @return an array of all input vectors
             */
            double *createData(const std::vector<std::valarray<double>> &vectors);

            /**
             * Creates a representation of the data used by VLFeat library
             * @param vector single data vector
             *
             * @return an array with data vector
             */
            inline double *createData(const std::valarray<double> &vector);
        };

    }

}


#endif //SPEECH_RECOGNITION_GAUSIANMIXTUREMODEL_H
