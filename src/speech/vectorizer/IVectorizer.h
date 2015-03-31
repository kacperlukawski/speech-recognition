#ifndef _SPEECH_RECOGNITION_IVECTORIZER_H_
#define _SPEECH_RECOGNITION_IVECTORIZER_H_

#include <vector>

#include "../raw_data/FrequencySample.h"

using speech::raw_data::FrequencySample;

namespace speech {

    namespace vectorizer {

        template<typename FrameType>
        class IVectorizer {
        public:
            virtual std::vector<double> vectorize(FrequencySample<FrameType> &sample) = 0;
        };

    }

}

#endif //_SPEECH_RECOGNITION_IVECTORIZER_H_
