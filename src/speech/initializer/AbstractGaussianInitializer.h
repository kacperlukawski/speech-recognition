#ifndef SPEECH_RECOGNITION_ABSTRACTGAUSSIANINITIALIZER_H
#define SPEECH_RECOGNITION_ABSTRACTGAUSSIANINITIALIZER_H

#include "../HMMLexicon.h"

namespace speech {

    namespace initializer {

        /**
         * Initialize the Gaussian HMM with initial estimates of means and variances
         */
        class AbstractGaussianInitializer {
        public:

            /**
             * Initializes the Gaussians of given HMM with means and variances
             * @param gaussianHmm instance of Gaussian HMM
             */
            virtual void initialize(speech::HMMLexicon::MultivariateGaussianHMM& gaussianHMM) = 0;
        };

    }

}

#endif //SPEECH_RECOGNITION_ABSTRACTGAUSSIANINITIALIZER_H
