//
// Created by kacper on 28.05.15.
//

#ifndef SPEECH_RECOGNITION_DISCRETEFOURIERTRANSFORM_H
#define SPEECH_RECOGNITION_DISCRETEFOURIERTRANSFORM_H

#include <complex>

using std::complex;

#include "IFrequencyTransform.h"

using speech::transform::IFrequencyTransform;

namespace speech {

    namespace transform {

        /**
         * Standard Discrete Fourier Transform with the complexity O(n^2)
         */
        template<typename FrameType>
        class DiscreteFourierTransform : public IFrequencyTransform<FrameType> {
        public:
            virtual FrequencySample<FrameType> transform(DataSample<FrameType> vector);
            
            virtual DataSample<FrameType> reverseTransform(FrequencySample<FrameType> vector);
        };

    }

}


#endif //SPEECH_RECOGNITION_DISCRETEFOURIERTRANSFORM_H
