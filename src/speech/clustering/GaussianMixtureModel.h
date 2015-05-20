//
// Created by kacper on 20.05.15.
//

#ifndef SPEECH_RECOGNITION_GAUSIANMIXTUREMODEL_H
#define SPEECH_RECOGNITION_GAUSIANMIXTUREMODEL_H

#include "IClusteringMethod.h"

extern "C" {
    #include "../../../lib/vl/generic.h"
}

namespace speech {

    namespace clustering {

        /**
         * Gaussian mixture model (GMM) implementation
         */
        class GaussianMixtureModel : public IClusteringMethod {
        public:
            virtual void fit(vector<valarray<double>> &vectors, vector<int> &labels);

            virtual int predict(const valarray<double> &vector);

            virtual void serialize(std::ostream &out) const;
        };

    }

}


#endif //SPEECH_RECOGNITION_GAUSIANMIXTUREMODEL_H
