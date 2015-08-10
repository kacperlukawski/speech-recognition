#include "DataSample.h"

#include <cstring>

template<typename FrameType>
speech::raw_data::DataSample<FrameType>::DataSample(int _size, int _length, std::shared_ptr<FrameType> _values) {
    this->size = _size;
    this->length = _length;
    this->values = _values;
}

template<typename FrameType>
speech::raw_data::DataSample<FrameType>::DataSample(const speech::raw_data::DataSample<FrameType> &ref) {
    this->size = ref.size;
    this->length = ref.length;
    this->values = std::shared_ptr<FrameType>(new FrameType[this->size], std::default_delete<FrameType[]>());

    memcpy(this->values.get(), ref.values.get(), size * sizeof(FrameType));
}

template<typename FrameType>
speech::raw_data::DataSample<FrameType>::~DataSample() {
    this->values.reset();
}

template<typename FrameType>
int speech::raw_data::DataSample<FrameType>::getSize() const {
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
