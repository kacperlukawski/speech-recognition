#include "WaveFileDataSource.h"

template<typename FrameType>
speech::raw_data::WaveFileDataSource<FrameType>::WaveFileDataSource(IFrequencyTransform<FrameType> *_frequencyDomainTransform, string _fileName)
        : DataSource<FrameType>(_frequencyDomainTransform) {
    fileName = _fileName;
}

template class speech::raw_data::WaveFileDataSource<signed char>;