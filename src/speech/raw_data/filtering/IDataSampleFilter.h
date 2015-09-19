#ifndef SPEECH_RECOGNITION_IDATASAMPLEFILTER_H
#define SPEECH_RECOGNITION_IDATASAMPLEFILTER_H

#include "../DataSample.h"

using speech::raw_data::DataSample;

namespace speech {

    namespace raw_data {

        namespace filtering {

            template<typename FrameType>
            class IDataSampleFilter {
            public:
                virtual DataSample<FrameType> filter(const DataSample<FrameType> &sample) = 0;
            };

        }

    }

}

#endif //SPEECH_RECOGNITION_IDATASAMPLEFILTER_H
