#ifndef _SPEECH_RECOGNITION_IFILTER_H_
#define _SPEECH_RECOGNITION_IFILTER_H_

#include "../FrequencySample.h"

using speech::raw_data::FrequencySample;

namespace speech {

    namespace raw_data {

        namespace filtering {

            template<typename FrameType>
            class IFilter {
            public:
                virtual FrequencySample<FrameType> filter(const FrequencySample<FrameType> &sample) = 0;
            };

        }

    }

}

#endif //_SPEECH_RECOGNITION_IFILTER_H_
