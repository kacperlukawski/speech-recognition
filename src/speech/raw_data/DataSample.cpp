#include "DataSample.h"

template<typename FrameType>
speech::raw_data::DataSample<FrameType>::DataSample(int _size, FrameType *_values) {
    size = _size;
    values = _values;
}

template<typename FrameType>
speech::raw_data::DataSample<FrameType>::~DataSample() {
    delete values;
}

speech::raw_data::DataSample<signed char> tmp(0, nullptr);