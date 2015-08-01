#ifndef IFREQUENCYTRANSFORM_H
#define IFREQUENCYTRANSFORM_H

#include "../raw_data/DataSample.h"
#include "../raw_data/FrequencySample.h"
#include "./window/Window.h"

using speech::raw_data::DataSample;
using speech::raw_data::FrequencySample;
using speech::transform::window::Window;

namespace speech {

    namespace transform {

        /**
        * This is an interface for all transforms which converts raw signal
        * into frequency domain.
        */
        template<typename FrameType>
        class IFrequencyTransform {
        public:
            virtual FrequencySample<FrameType> transform(DataSample<FrameType> vector) = 0;

            virtual DataSample<FrameType> reverseTransform(FrequencySample<FrameType> vector) = 0;

            virtual FrequencySample<FrameType> transform(DataSample<FrameType> vector, Window* window) = 0;
        };
    }

}

#endif