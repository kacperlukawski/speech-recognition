//
// Created by kacper on 19.05.15.
//

#ifndef SPEECH_RECOGNITION_MFCCVECTORIZER_H
#define SPEECH_RECOGNITION_MFCCVECTORIZER_H

#include <cmath>

#include "../transform/IFrequencyTransform.h"
#include "../transform/FastFourierTransform.h"

using speech::transform::IFrequencyTransform;
using speech::transform::FastFourierTransform;

#include "IVectorizer.h"

namespace speech {

    namespace vectorizer {

        /**
         * Vectorizer using Mel Frequency Capstral Coefficients to
         * describe a sound sample.
         * @see http://webapp.etsi.org/workprogram/Report_WorkItem.asp?wki_id=18820
         */
        template<typename FrameType>
        class MFCCVectorizer : public IVectorizer<FrameType> {
        public:
            MFCCVectorizer(int bins, int cepstralCoefficientsNumber) :
                    bins(bins), cepstralCoefficientsNumber(cepstralCoefficientsNumber) { }

            virtual std::valarray<double> vectorize(FrequencySample<FrameType> &sample);

            virtual std::vector<std::valarray<double>> vectorize(
                    std::vector<FrequencySample<FrameType>> &samples) override;

            virtual void serialize(std::ostream &out) const;

            virtual int getVectorSize() const;

        private:
            int bins;
            int cepstralCoefficientsNumber;

            /**
             * Calculates the frequency on mel scale
             * @see http://en.wikipedia.org/wiki/Mel_scale
             * @param frequency frequency in Hz
             *
             * @return frequency in mels
             */
            inline double getMelFrequency(double frequency) {
                return 1127.0 * log(1 + frequency / 700.0);
            }

            /**
             * Converts mels into Hz
             * @see http://en.wikipedia.org/wiki/Mel_scale
             * @param mels frequency in mels
             *
             * @return frequency in Hz
             */
            inline double getFrequencyFromMels(double mels) {
                return 700 * (exp(mels / 1127.0) - 1);
            }

            /**
             * Calculates the cepstrum of the given sample
             * @see http://www.phon.ucl.ac.uk/courses/spsci/matlab/lect10.html
             */
            DataSample<FrameType> getCepstrum(const FrequencySample<FrameType>&sample);

            /** Transform between time and frequency domain */
            IFrequencyTransform<FrameType>* frequencyTransform = new FastFourierTransform<FrameType>(); // TODO: make it more dynamic
        };

    }

}


#endif //SPEECH_RECOGNITION_MFCCVECTORIZER_H
