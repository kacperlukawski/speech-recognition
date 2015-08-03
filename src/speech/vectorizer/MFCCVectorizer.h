//
// Created by kacper on 19.05.15.
//

#ifndef SPEECH_RECOGNITION_MFCCVECTORIZER_H
#define SPEECH_RECOGNITION_MFCCVECTORIZER_H

#include <istream>
#include <cmath>

#include "../raw_data/Spectrum.h"

using speech::raw_data::Spectrum;

#include "../transform/IFrequencyTransform.h"
#include "../transform/FastFourierTransform.h"
#include "../transform/DiscreteFourierTransform.h"

using speech::transform::IFrequencyTransform;
using speech::transform::FastFourierTransform;
using speech::transform::DiscreteFourierTransform;

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
            /** Globally unique type identifier of this vectorizer */
            static const uint32_t TYPE_IDENTIFIER = 0x00100004;

            /**
             * Construct the vectorizer from a data stream containing serialized vectorizer
             * @param in input data stream
             */
            MFCCVectorizer(std::istream &in);

            /**
             * Construct the vectorizer using given number of bins and cepstral coefficients
             * @param bins number of bins
             * @param cepstralCoefficientsNumber number of cepstral coefficients
             * @param minFrequency
             * @param maxFrequency
             */
            MFCCVectorizer(int bins, int cepstralCoefficientsNumber, double minFrequency = 64.0,
                           double maxFrequency = 16000.0, int windowMsSize = 20, int offsetMsSize = 10) :
                    bins(bins),
                    cepstralCoefficientsNumber(cepstralCoefficientsNumber),
                    minCepstrumFrequency(minFrequency),
                    maxCepstrumFrequency(maxFrequency),
                    windowMsSize(windowMsSize),
                    offsetMsSize(offsetMsSize) {
                buildFilterBank();
            }

            virtual std::valarray<double> vectorize(FrequencySample<FrameType> &sample);

            virtual std::vector<std::valarray<double>> vectorize(
                    std::vector<FrequencySample<FrameType>> &samples) override;

            virtual std::vector<std::valarray<double>> vectorize(DataSource<FrameType> &dataSource) override;

            virtual void serialize(std::ostream &out) const;

            virtual int getVectorSize() const;

        protected:
            /** Declaration of the MelFilter class used for creating bank of filters */
            class MelFilter;

            /** Number of mel frequency bins */
            int bins;

            /** Number of used cepstral coefficients */
            int cepstralCoefficientsNumber;

            /** Minimal frequency in cepstrum calculating */
            double minCepstrumFrequency;

            /** Maximal frequency in cepstrum calculating */
            double maxCepstrumFrequency;

            /** Transform between time and frequency domain */
            IFrequencyTransform<FrameType> *frequencyTransform = new FastFourierTransform<FrameType>();//  new DiscreteFourierTransform<FrameType>(); // TODO: should be more dynamic

            /** Bank of Mel filters */
            std::vector<MelFilter> filterBank;

            /**
             * Calculates the frequency on mel scale
             * @see http://en.wikipedia.org/wiki/Mel_scale
             * @param frequency frequency in Hz
             *
             * @return frequency in mels
             */
            inline double herzToMel(double frequency) {
                return 1127.0 * log(1 + frequency / 700.0);
            }

            /**
             * Converts mels into Hz
             * @see http://en.wikipedia.org/wiki/Mel_scale
             * @param mels frequency in mels
             *
             * @return frequency in Hz
             */
            inline double melToHerz(double mels) {
                return 700 * (exp(mels / 1127.0) - 1);
            }

            /**
             * Calculates the cepstrum of the given sample
             * @see http://www.phon.ucl.ac.uk/courses/spsci/matlab/lect10.html
             */
            std::valarray<double> calculateCepstrumCoefficients(const FrequencySample<FrameType> &sample);

            /**
             * A triangular filter used in MFCC calculating
             */
            class MelFilter {
            public:
                /**
                 * Constructs a filter non-zero in given boundaries
                 * @param startFrequency begin frequency in herzs
                 * @param endFrequency end frequency in herzs
                 */
                MelFilter(double startFrequency, double endFrequency)
                        : startFrequency(startFrequency), endFrequency(endFrequency) { }

                /**
                 * Calucates an energy in given spectrum
                 * @param spectrum reference to Spectrum class object
                 * @return energy of this filter in given spectrum
                 */
                double operator()(Spectrum &spectrum) {
                    int startIndex = spectrum.getIndexByFrequency(startFrequency);
                    int endIndex = spectrum.getIndexByFrequency(endFrequency);
                    int centerIndex = (startIndex + endIndex) / 2;

                    double result = 0.0;
                    std::valarray<double> &spectrumValues = spectrum.getValues();
                    for (int index = startIndex; index < endIndex; index++) {
                        if (index < centerIndex) {
                            result += spectrumValues[index] *
                                      (double) ((index - startIndex) / (centerIndex - startIndex));
                        } else {
                            result += spectrumValues[index] * (double) ((endIndex - index) / (endIndex - centerIndex));
                        }
                    }

                    return result;
                }

            private:
                /** Begin index */
                double startFrequency;
                /** End index */
                double endFrequency;
            };

        private:
            static constexpr double EPS = 10e-12;
            /** Size of a single window in milliseconds */
            int windowMsSize = 20;
            /** The offset between two frames in milliseconds */
            int offsetMsSize = 10;

            /**
             * Creates a bank of filters used in vectorizing
             */
            void buildFilterBank();
        };

    }

}


#endif //SPEECH_RECOGNITION_MFCCVECTORIZER_H
