//
// Created by kacper on 18.05.15.
//

#ifndef SPEECH_RECOGNITION_HARMONICSVECTORIZER_H
#define SPEECH_RECOGNITION_HARMONICSVECTORIZER_H

#include "IVectorizer.h"

namespace speech {

    namespace vectorizer {

        template<typename FrameType>
        class HarmonicsVectorizer: public IVectorizer<FrameType> {
        public:
            /**
             * Creates a new vectorizer founding the harmonics of samples.
             * @param maxHarmonics maximal number of the harmonics
             */
            HarmonicsVectorizer(int maxHarmonics) : maxHarmonics(maxHarmonics) {}

            virtual std::valarray<double> vectorize(FrequencySample<FrameType> &sample);

            virtual void serialize(std::ostream &out) const;

            virtual int getVectorSize() const;
        private:
            static double constexpr EPS = 10e-2;

            int maxHarmonics;
        };

    }

}


#endif //SPEECH_RECOGNITION_HARMONICSVECTORIZER_H
