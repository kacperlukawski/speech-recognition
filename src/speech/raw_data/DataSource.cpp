#include "DataSource.h"

template<typename FrameType>
speech::raw_data::DataSource<FrameType>::DataSource(IFrequencyTransform<FrameType> *_frequencyDomainTransform) {
    samples = new list<DataSample<FrameType>>();
    frequencyDomainTransform = _frequencyDomainTransform;
}

