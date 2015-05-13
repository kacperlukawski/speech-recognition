#ifndef FREQUENCYSAMPLE_H
#define FREQUENCYSAMPLE_H

#include <memory>

namespace speech {

    namespace raw_data {

        /**
         * This class represents a sample of raw data, but in the frequencies' domain
         */
        template<typename FrameType>
        class FrequencySample {
        protected:
            /** Size of the digital sound sample */
            int size;
            /** Length of the sample in milliseconds */
            int length;
            std::shared_ptr<double> amplitude;
            std::shared_ptr<double> phase;
        public:
            FrequencySample(int _size, int _length, std::shared_ptr<double> _amplitude, std::shared_ptr<double> _phase);

            FrequencySample(const FrequencySample<FrameType> &ref);

            virtual ~FrequencySample();

            int getSize() const;

            int getLength();

            std::shared_ptr<double> getAmplitude();

            std::shared_ptr<double> getPhase();

            /**
             * Get minimal frequency which can be represented by this sample.
             * Minimal frequency is a frequency with the highest period.
             * This frequency is stored on the first position in the sample
             * (position 0 is reserved for a constant).
             * @see http://s-mat-pcs.oulu.fi/~ssa/ESignals/sig2_2_4.htm
             *
             * @return frequency in Hz
             */
            inline double getMinFrequency() {
                return 1 / (length / 1000.0);
            }

            /**
             * Get maximal frequency which can be represented by this sample.
             * Maximal frequency is a frequency with the lowest period.
             * This frequency is stored on the center position in the sample.
             * @see http://s-mat-pcs.oulu.fi/~ssa/ESignals/sig2_2_4.htm
             *
             * @return frequency in Hz
             */
            inline double getMaxFrequency() {
                return size / (2.0 * length / 1000.0);
            }

            /**
             * Get the frequency stored on given index of this sample.
             * @param index position in the array of values
             *
             * @return frequency in Hz
             */
            inline double getIndexFrequency(int index) {
                int halfSize = size / 2;
                if (index > halfSize) {
                    // sample has duplicates of frequencies, so let
                    // consider just first half of the sample
                    index = halfSize - index;
                }

                return getMinFrequency() * index;
            }

            /**
             * Get the index which stores at most given frequency.
             * @param frequency frequency in Hz
             *
             * @return index in the array of values
             */
            inline int getFrequencyIndex(double frequency) {
                return (int) (frequency / getMinFrequency());
            }

        };

    }

}

#endif