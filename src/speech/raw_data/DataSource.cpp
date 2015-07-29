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
typename speech::raw_data::DataSource<FrameType>::iterator speech::raw_data::DataSource<FrameType>::getSamplesOffsetIteratorEnd(
        int windowSizeInMilliseconds, int offsetInMilliseconds) {
    return DataSource::iterator(*this, samples->end(), offsetInMilliseconds, windowSizeInMilliseconds, true);
}

template<typename FrameType>
typename speech::raw_data::DataSource<FrameType>::iterator speech::raw_data::DataSource<FrameType>::getSamplesOffsetIteratorBegin(
        int windowSizeInMilliseconds, int offsetInMilliseconds) {
    return DataSource<FrameType>::iterator(*this, samples->begin(), offsetInMilliseconds, windowSizeInMilliseconds);
}

template
class speech::raw_data::DataSource<unsigned char>;

template
class speech::raw_data::DataSource<short int>;