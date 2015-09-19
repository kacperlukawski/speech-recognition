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
void speech::raw_data::DataSource<FrameType>::init() {
}

template<typename FrameType>
typename speech::raw_data::DataSource<FrameType>::normalIterator speech::raw_data::DataSource<FrameType>::getSamplesIteratorBegin() {
    return normalIterator(samples->begin());
}

template<typename FrameType>
typename speech::raw_data::DataSource<FrameType>::normalIterator speech::raw_data::DataSource<FrameType>::getSamplesIteratorEnd() {
    return normalIterator(samples->end());
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