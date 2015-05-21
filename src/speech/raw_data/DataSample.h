#ifndef DATASAMPLE_H
#define DATASAMPLE_H

#include <memory>

namespace speech {

    namespace raw_data {

        /**
         * This class represents a sample of the raw data taken from the data source.
         * It has fixed size and stores digital sound.
         */
        template<typename FrameType>
        class DataSample {
        protected:
            /** Size of the digital sound sample */
            int size;
            /** Length of the sample in milliseconds */
            int length;
            std::shared_ptr<FrameType> values;
        public:
            DataSample(int _size, int _length, std::shared_ptr<FrameType> _values);

            DataSample(const DataSample<FrameType>& ref);

            virtual ~DataSample();

            int getSize();

            int getLength();

            std::shared_ptr<FrameType> getValues();
        };

    }

}

#endif