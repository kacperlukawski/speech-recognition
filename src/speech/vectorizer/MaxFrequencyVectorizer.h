#ifndef _SPEECH_RECOGNITION_MAXFREQUENCYVECTORIZER_H_
#define _SPEECH_RECOGNITION_MAXFREQUENCYVECTORIZER_H_

#include "IVectorizer.h"

#include "../raw_data/filtering/MaxFrequenciesFilter.h"

using speech::raw_data::filtering::MaxFrequenciesFilter;

namespace speech {

    namespace vectorizer {

        /**
         * Vectorizer converting raw signal into vectors of fixed size
         * containing frequencies with maximal amplitude.
         * @deprecated
         */
        template<typename FrameType>
        class MaxFrequencyVectorizer : public IVectorizer<FrameType> {
        public:
            static const uint32_t TYPE_IDENTIFIER = 0x00100001;

            MaxFrequencyVectorizer(int n);

            MaxFrequencyVectorizer(std::istream &in);

            virtual std::valarray<double> vectorize(FrequencySample<FrameType> &sample) override;

            virtual void serialize(std::ostream &out) const override;


            virtual int getVectorSize() const;

        private:
            int n;

            MaxFrequenciesFilter<FrameType> *filterPtr;
        };

    }

}


#endif //_SPEECH_RECOGNITION_MAXFREQUENCYVECTORIZER_H_
