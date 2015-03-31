#ifndef _SPEECH_RECOGNITION_MAXFREQUENCIESFILTER_H_
#define _SPEECH_RECOGNITION_MAXFREQUENCIESFILTER_H_

#include "IFilter.h"

using speech::raw_data::filtering::IFilter;

namespace speech {

    namespace raw_data {

        namespace filtering {

            template<typename FrameType>
            class MaxFrequenciesFilter : public IFilter<FrameType> {
            public:
                MaxFrequenciesFilter(int n);

                virtual FrequencySample<FrameType> filter(const FrequencySample<FrameType> &sample);

            protected:
                int n;

                double getNthMaxValue(FrequencySample<FrameType> &sample);
            };

        }

    }

}

#endif //_SPEECH_RECOGNITION_MAXFREQUENCIESFILTER_H_
