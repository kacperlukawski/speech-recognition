//
// Created by kacper on 19.04.15.
//

#ifndef SPEECH_RECOGNITION_EUCLIDEANDISTANCE_H
#define SPEECH_RECOGNITION_EUCLIDEANDISTANCE_H

#include "IMetric.h"

namespace speech {

    namespace metric {

        class EuclideanDistance : public IMetric {
        public:

            virtual double operator()(const std::valarray<double> &v1,
                                      const std::valarray<double> &v2) override;
        };

    }

}

#endif //SPEECH_RECOGNITION_EUCLIDEANDISTANCE_H
