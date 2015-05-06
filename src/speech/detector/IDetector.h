//
// Created by kacper on 06.05.15.
//

#ifndef SPEECH_RECOGNITION_IDETECTOR_H
#define SPEECH_RECOGNITION_IDETECTOR_H

#include "../raw_data/FrequencySample.h"

using speech::raw_data::FrequencySample;

namespace speech {

    namespace detector {

        template<typename FrameType>
        class IDetector {
        public:
            ///
            /// Checks the condition of the particular detector, and returns
            /// TRUE if the condition was met, FALSE otherwise.
            ///
            virtual bool detected(FrequencySample<FrameType> &sample) const = 0;
        };

    }

}

#endif //SPEECH_RECOGNITION_IDETECTOR_H
