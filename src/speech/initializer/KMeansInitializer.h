//
// Created by kacper on 31.08.15.
//

#ifndef SPEECH_RECOGNITION_KMEANSINITIALIZER_H
#define SPEECH_RECOGNITION_KMEANSINITIALIZER_H

#include "../clustering/KMeans.h"

#include "RandomInitializer.h"

namespace speech {

    namespace initializer {

        /**
         * Initializer choosing initial estimates using KMeans clustering
         */
        class KMeansInitializer : public RandomInitializer {
        public:
            virtual void initialize(speech::HMMLexicon::MultivariateGaussianHMM &gaussianHMM);
        };

    }

}


#endif //SPEECH_RECOGNITION_KMEANSINITIALIZER_H
