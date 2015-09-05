//
// Created by kacper on 19.05.15.
//

#include "EmphasisFilter.h"

template<typename FrameType>
DataSample<FrameType> speech::raw_data::filtering::EmphasisFilter<FrameType>::filter(const DataSample<FrameType> &sample) {
    DataSample<FrameType> result(sample);
    FrameType *sampleValues = result.getValues().get();

    int sampleSize = result.getSize();
    for (int i = sampleSize - 1; i > 0; i--) {
        sampleValues[i] -= sampleValues[i - 1] * this->alpha;
    }

    return result;
}

template
class speech::raw_data::filtering::EmphasisFilter<short>;