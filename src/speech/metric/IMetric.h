#ifndef SPEECH_RECOGNITION_IMETRIC_H
#define SPEECH_RECOGNITION_IMETRIC_H

#include <valarray>

namespace speech {

    namespace metric {

        class IMetric {
        public:

            /**
             * Calculates a distance between two vectors.
             * @return a distance
             */
            virtual double operator()(const std::valarray<double> &v1,
                                      const std::valarray<double> &v2) = 0;
        };

    }

}

#endif //SPEECH_RECOGNITION_IMETRIC_H
