#include "FrequencySample.h"

template<typename FrameType>
speech::raw_data::FrequencySample<FrameType>::FrequencySample(int _size, double *_amplitude, double *_phase) {
    size = _size;
    amplitude = _amplitude;
    phase = _phase;
}

template<typename FrameType>
speech::raw_data::FrequencySample<FrameType>::~FrequencySample() {
    // @TODO
//    if (amplitude != nullptr) {
//        delete amplitude;
//    }
//    if (phase != nullptr) {
//        delete phase;
//    }
}

template<typename FrameType>
int speech::raw_data::FrequencySample<FrameType>::getSize() {
    return size;
}

template<typename FrameType>
double *speech::raw_data::FrequencySample<FrameType>::getAmplitude() {
    return amplitude;
}

template<typename FrameType>
double *speech::raw_data::FrequencySample<FrameType>::getPhase() {
    return phase;
}

template
class speech::raw_data::FrequencySample<short>;