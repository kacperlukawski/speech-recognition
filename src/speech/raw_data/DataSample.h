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
            int size;
            std::shared_ptr<FrameType> values;
        public:
            DataSample(int _size, std::shared_ptr<FrameType> _values);
            virtual ~DataSample();

            int getSize();
            std::shared_ptr<FrameType> getValues();
        };

    }

}

#endif