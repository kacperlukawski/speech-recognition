#ifndef SPEECH_RECOGNITION_RANDOMINITIALIZER_H
#define SPEECH_RECOGNITION_RANDOMINITIALIZER_H

#include "AbstractGaussianInitializer.h"

namespace speech {

    namespace initializer {

        class RandomInitializer : public AbstractGaussianInitializer {
        public:
            virtual void initialize(speech::HMMLexicon::MultivariateGaussianHMM &gaussianHMM);

        protected:
            unsigned int countObservations(std::vector<HMMLexicon::Observation> *utterances);

            std::valarray<double> calculateMean(std::vector<HMMLexicon::Observation> *utterances, int dimensionality,
                                                int observationsNb);

            std::valarray<double> calculateVariance(std::vector<HMMLexicon::Observation> *utterances,
                                                    int dimensionality, int observationsNb);
        };

    }

}


#endif //SPEECH_RECOGNITION_RANDOMINITIALIZER_H
