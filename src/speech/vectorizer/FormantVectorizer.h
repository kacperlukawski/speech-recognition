//
// Created by kacper on 10.05.15.
//

#ifndef SPEECH_RECOGNITION_FORMANTVECTORIZER_H
#define SPEECH_RECOGNITION_FORMANTVECTORIZER_H

#include <istream>

#include "IVectorizer.h"

namespace speech {

    namespace vectorizer {

        /**
         * Vectorizer getting formants from samples and
         * converting them into the vector space.
         */
        template<typename FrameType>
        class FormantVectorizer : public IVectorizer<FrameType> {

        public:
            static const uint32_t TYPE_IDENTIFIER = 0x00100002;

            FormantVectorizer(std::istream &in);

            virtual std::valarray<double> vectorize(FrequencySample<FrameType> &sample);

            virtual void serialize(std::ostream &out) const;
        };

    }

}

#endif //SPEECH_RECOGNITION_FORMANTVECTORIZER_H
