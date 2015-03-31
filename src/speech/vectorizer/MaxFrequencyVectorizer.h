#ifndef _SPEECH_RECOGNITION_MAXFREQUENCYVECTORIZER_H_
#define _SPEECH_RECOGNITION_MAXFREQUENCYVECTORIZER_H_

#include "IVectorizer.h"

#include "../raw_data/filtering/MaxFrequenciesFilter.h"

using speech::raw_data::filtering::MaxFrequenciesFilter;

namespace speech {

    namespace vectorizer {

        template<typename FrameType>
        class MaxFrequencyVectorizer : public IVectorizer<FrameType> {
        public:
            MaxFrequencyVectorizer(int n);

            virtual std::vector<double> vectorize(FrequencySample<FrameType> &sample) override;

        private:
            MaxFrequenciesFilter<FrameType> *filterPtr;
        };

    }

}


#endif //_SPEECH_RECOGNITION_MAXFREQUENCYVECTORIZER_H_
