#ifndef _SPEECH_RECOGNITION_IVECTORIZER_H_
#define _SPEECH_RECOGNITION_IVECTORIZER_H_

#include <valarray>

#include "../IStreamSerializable.h"

using speech::IStreamSerializable;

#include "../raw_data/FrequencySample.h"

using speech::raw_data::FrequencySample;

namespace speech {

    namespace vectorizer {

        template<typename FrameType>
        class IVectorizer : public IStreamSerializable {
        public:
            //
            // Projects given sample into feature space. Each vector needs to have
            // exactly same size, equal to dimension of the feature space.
            // @return the projection of vector in a feature space
            //
            virtual std::valarray<double> vectorize(FrequencySample<FrameType> &sample) = 0;

            virtual void serialize(std::ostream &out) const = 0;
        };

    }

}

#endif //_SPEECH_RECOGNITION_IVECTORIZER_H_
