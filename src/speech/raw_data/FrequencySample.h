#ifndef FREQUENCYSAMPLE_H
#define FREQUENCYSAMPLE_H

namespace speech {

    namespace raw_data {

        /**
         * This class represents a sample of raw data, but in the frequencies' domain
         */
        template<typename FrameType>
        class FrequencySample {
        protected:
            int size;
            double *amplitude;
            double *phase;
        public:
            FrequencySample(int _size, double *_amplitude, double *_phase);

            ~FrequencySample();
        };

    }

}

#endif