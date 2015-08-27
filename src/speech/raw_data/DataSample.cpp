#include "DataSample.h"

#include <cstring>

template<typename FrameType>
speech::raw_data::DataSample<FrameType>::DataSample(int _size, int _length, const std::shared_ptr<FrameType> &_values)
        : values(_values) {
    this->size = _size;
    this->length = _length;
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
int speech::raw_data::DataSample<FrameType>::getLength() const {
    return this->length;
}

template<typename FrameType>
const std::shared_ptr<FrameType> speech::raw_data::DataSample<FrameType>::getValues() const {
    return values;
}

template
class speech::raw_data::DataSample<unsigned char>;

template
class speech::raw_data::DataSample<short int>;
