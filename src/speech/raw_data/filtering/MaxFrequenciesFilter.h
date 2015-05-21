#ifndef _SPEECH_RECOGNITION_MAXFREQUENCIESFILTER_H_
#define _SPEECH_RECOGNITION_MAXFREQUENCIESFILTER_H_

#include "IFrequencySampleFilter.h"

using speech::raw_data::filtering::IFrequencySampleFilter;

namespace speech {

    namespace raw_data {

        namespace filtering {

            template<typename FrameType>
            class MaxFrequenciesFilter : public IFrequencySampleFilter<FrameType> {
            public:
                MaxFrequenciesFilter(int n);

                virtual FrequencySample<FrameType> filter(const FrequencySample<FrameType> &sample);

            protected:
                static double constexpr EPS = 1e-2;

                int n;
            };

        }

    }

}

#endif //_SPEECH_RECOGNITION_MAXFREQUENCIESFILTER_H_
