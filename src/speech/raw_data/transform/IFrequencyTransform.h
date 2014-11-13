#ifndef IFREQUENCYTRANSFORM_H
#define IFREQUENCYTRANSFORM_H

#include "../DataSample.h"

using speech::raw_data::DataSample;

#include "../FrequencySample.h"

using speech::raw_data::FrequencySample;

namespace speech {

    namespace raw_data {

        namespace transform {

            /**
            * This is an interface for all transforms which converts raw signal
            * into frequency domain.
            */
            template<typename FrameType>
            class IFrequencyTransform {
                virtual FrequencySample<FrameType> transform(DataSample<FrameType> vector) = 0;

                virtual DataSample<FrameType> reverseTransform(FrequencySample<FrameType> vector) = 0;
            };

        }

    }

}

#endif