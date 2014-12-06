#include "DataSample.h"

template<typename FrameType>
speech::raw_data::DataSample<FrameType>::DataSample(int _size, FrameType *_values) {
    size = _size;
    this->values = _values;
}

template<typename FrameType>
speech::raw_data::DataSample<FrameType>::~DataSample() {
    // @TODO
    //  delete values;
}

template<typename FrameType>
int speech::raw_data::DataSample<FrameType>::getSize() {
    return this->size;
}

template<typename FrameType>
FrameType *speech::raw_data::DataSample<FrameType>::getValues() {
    return values;
}

template
class speech::raw_data::DataSample<unsigned char>;

template
class speech::raw_data::DataSample<short int>;
