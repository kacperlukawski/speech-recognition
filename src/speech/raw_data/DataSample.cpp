#include "DataSample.h"

template<typename FrameType>
speech::raw_data::DataSample<FrameType>::DataSample(int _size, int _length, std::shared_ptr<FrameType> _values) {
    size = _size;
    this->values = _values;
}

template<typename FrameType>
speech::raw_data::DataSample<FrameType>::~DataSample() {
    this->values.reset();
}

template<typename FrameType>
int speech::raw_data::DataSample<FrameType>::getSize() {
    return this->size;
}

template<typename FrameType>
int speech::raw_data::DataSample<FrameType>::getLength() {
    return this->length;
}

template<typename FrameType>
std::shared_ptr<FrameType> speech::raw_data::DataSample<FrameType>::getValues() {
    return values;
}

template
class speech::raw_data::DataSample<unsigned char>;

template
class speech::raw_data::DataSample<short int>;
