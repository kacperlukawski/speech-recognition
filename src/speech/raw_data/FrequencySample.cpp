#include "FrequencySample.h"

template<typename FrameType>
speech::raw_data::FrequencySample<FrameType>::FrequencySample(int _size, double *_amplitude, double *_phase) {
    size = size;
    amplitude = _amplitude;
    phase = _phase;
}

template<typename FrameType>
speech::raw_data::FrequencySample<FrameType>::~FrequencySample() {
    delete amplitude;
    delete phase;
}
