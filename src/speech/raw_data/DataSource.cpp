#include "DataSource.h"

template<typename FrameType>
speech::raw_data::DataSource<FrameType>::DataSource() {
    samples = new list<DataSample<FrameType>>();
}

template class speech::raw_data::DataSource<signed char>;