#ifndef SPEECH_RECOGNITION_MAXIMUMDISTANCE_H
#define SPEECH_RECOGNITION_MAXIMUMDISTANCE_H

#include "IMetric.h"

namespace speech {

    namespace metric {

        /**
         * A metric calculating maximum distance between two given vectors:
         * d(x, y) = max_i(|x_i - y_i|)
         */
        class MaximumDistance : public IMetric {
        public:
            virtual double operator()(const std::valarray<double> &v1,
                                      const std::valarray<double> &v2);
        };

    }

}

#endif //SPEECH_RECOGNITION_MAXIMUMDISTANCE_H
