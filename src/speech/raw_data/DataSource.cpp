#include "DataSource.h"

template<typename FrameType>
speech::raw_data::DataSource<FrameType>::DataSource() {
    samples = std::make_shared<list<DataSample<FrameType>>>();
}

template<typename FrameType>
speech::raw_data::DataSource<FrameType>::~DataSource() {
    samples.reset();
}

template<typename FrameType>
void speech::raw_data::DataSource<FrameType>::addSample(DataSample<FrameType> sample) {
    samples->push_back(sample);
}

template<typename FrameType>
typename list<DataSample<FrameType>>::iterator speech::raw_data::DataSource<FrameType>::getSamplesIteratorBegin() {
    return samples->begin();
}

template<typename FrameType>
typename list<DataSample<FrameType>>::iterator speech::raw_data::DataSource<FrameType>::getSamplesIteratorEnd() {
    return samples->end();
}

template
class speech::raw_data::DataSource<unsigned char>;

template
class speech::raw_data::DataSource<short int>;