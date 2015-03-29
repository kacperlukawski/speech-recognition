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
            int size;
            std::shared_ptr<double> amplitude;
            std::shared_ptr<double> phase;
        public:
            FrequencySample(int _size, std::shared_ptr<double> _amplitude, std::shared_ptr<double> _phase);

            FrequencySample(const FrequencySample<FrameType> &ref);

            virtual ~FrequencySample();

            int getSize() const;

            std::shared_ptr<double> getAmplitude();

            std::shared_ptr<double> getPhase();
        };

    }

}

#endif