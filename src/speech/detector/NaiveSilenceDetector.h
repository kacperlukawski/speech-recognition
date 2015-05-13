//
// Created by kacper on 06.05.15.
//

#ifndef SPEECH_RECOGNITION_NAIVESILENCEDETECTOR_H
#define SPEECH_RECOGNITION_NAIVESILENCEDETECTOR_H

#include "IDetector.h"

namespace speech {

    namespace detector {

        ///
        /// The detector of silence based on some intuitions only.
        /// It checks the mean and standard deviation of the signal
        /// amplitude with thresholding. Silence tends to use some
        /// high frequencies (noise) and using this intution we
        /// try to detect it.
        ///
        template<typename FrameType>
        class NaiveSilenceDetector : public IDetector<FrameType> {
        public:

            virtual bool detected(FrequencySample<FrameType> &sample) const;
        };

    }

}

#endif //SPEECH_RECOGNITION_NAIVESILENCEDETECTOR_H
