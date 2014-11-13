#include "WaveFileDataSource.h"

template<typename FrameType>
speech::raw_data::WaveFileDataSource<FrameType>::WaveFileDataSource(IFrequencyTransform<FrameType> *_frequencyDomainTransform, string _fileName)
        : DataSource(_frequencyDomainTransform) {
    fileName = _fileName;
}