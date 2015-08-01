#include "DataSource.h"

template<typename FrameType>
speech::raw_data::DataSource<FrameType>::DataSource(int sampleLength) {
    samples = std::make_shared<list<DataSample<FrameType>>>();
    this->sampleLength = sampleLength;
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

template<typename FrameType>
typename speech::raw_data::DataSource<FrameType>::offsetIterator speech::raw_data::DataSource<FrameType>::getOffsetIteratorEnd() {
    return DataSource::offsetIterator(*this, 0, 1, true);
}

template<typename FrameType>
typename speech::raw_data::DataSource<FrameType>::offsetIterator speech::raw_data::DataSource<FrameType>::getOffsetIteratorBegin(
        int windowSizeInMilliseconds, int offsetInMilliseconds) {
    return DataSource<FrameType>::offsetIterator(*this, offsetInMilliseconds, windowSizeInMilliseconds);
}

template
class speech::raw_data::DataSource<unsigned char>;

template
class speech::raw_data::DataSource<short int>;