#include "WaveFileDataSource.h"

template<typename FrameType>
speech::raw_data::WaveFileDataSource<FrameType>::WaveFileDataSource(string _fileName)
        : DataSource<FrameType>() {
    fileName = _fileName;
}

template class speech::raw_data::WaveFileDataSource<signed char>;