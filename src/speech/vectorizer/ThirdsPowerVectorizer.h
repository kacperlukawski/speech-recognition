//
// Created by kacper on 18.05.15.
//

#ifndef SPEECH_RECOGNITION_THIRDSPOWERVECTORIZER_H
#define SPEECH_RECOGNITION_THIRDSPOWERVECTORIZER_H

#include "IVectorizer.h"

namespace speech {

    namespace vectorizer {

        template<typename FrameType>
        class ThirdsPowerVectorizer : public IVectorizer<FrameType> {
        public:
            static const uint32_t TYPE_IDENTIFIER = 0x00100003;

            ThirdsPowerVectorizer() {}

            ThirdsPowerVectorizer(std::istream &istream);

            virtual std::valarray<double> vectorize(FrequencySample<FrameType> &sample);

            virtual void serialize(std::ostream &out) const;

            virtual int getVectorSize() const;
        protected:
            /**
             * Calculates an average energy of the third hidden behind given indexes
             * @return double
             */
            double getThirdEnergy(double *amplitudePtr, int lowerIndex, int upperIndex);
        private:
            static int const THIRDS_MIDDLE_FREQUENCIES_COUNT = 24; // number of the frequencies below
            static double const THIRDS_MIDDLE_FREQUENCIES[THIRDS_MIDDLE_FREQUENCIES_COUNT];  // middle frequencies of the acoustic thirds, three thirds create an octave
        };

    }

}


#endif //SPEECH_RECOGNITION_THIRDSPOWERVECTORIZER_H
