#ifndef DATASAMPLE_H
#define DATASAMPLE_H 1

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
            FrameType *values;
        public:
            DataSample(int _size, FrameType *_values);

            ~DataSample();
        };

    }

}

#endif