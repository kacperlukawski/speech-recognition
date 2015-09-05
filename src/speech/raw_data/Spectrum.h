//
// Created by kacper on 30.05.15.
//

#ifndef SPEECH_RECOGNITION_SPECTRUM_H
#define SPEECH_RECOGNITION_SPECTRUM_H

#include <cmath>
#include <valarray>

#include "FrequencySample.h"

namespace speech {

    namespace raw_data {

        /**
         * Spectrum of the sound sample
         */
        class Spectrum {
        public:
            /**
             * Constructs a spectrum of given sample
             * @param sample sound sample in frequency domain
             */
            template<typename FrameType>
            Spectrum(const FrequencySample<FrameType> &sample)
                    : values(std::valarray<double>(0.0, sample.getSize())),
                      basicFrequency(sample.getMinFrequency()) {
                int size = sample.getSize();
                double *amplitudePtr = sample.getAmplitude().get();
                for (int i = 0; i < size; i++) {
                    this->values[i] = pow(amplitudePtr[i], 2.0);
                }
            }

            /**
             * Destructor
             */
            virtual ~Spectrum() { }

            /**
             * Returns values of the spectrum for given sample
             * @return spectrum values
             */
            std::valarray<double> &getValues() {
                return this->values;
            }

            /**
             * Gets the index of cell containing the value
             * of frequency closest to given frequency
             * @param frequency frequency in Herzs
             * @return index containg this value
             */
            inline int getIndexByFrequency(double frequency) const {
                return (int) ceil(frequency / basicFrequency);
            }

            /**
             * Gets the frequency stored in the given index
             * @param index index to check
             * @return frequency in given index in Herzs
             */
            inline double getIndexFrequency(int index) const {
                return index * basicFrequency;
            }

        protected:
            /** Squared magnitude (amplitude) of given sample */
            std::valarray<double> values;

            /** A frequency of the first amplitude */
            double basicFrequency;
        };

    }

}


#endif //SPEECH_RECOGNITION_SPECTRUM_H
