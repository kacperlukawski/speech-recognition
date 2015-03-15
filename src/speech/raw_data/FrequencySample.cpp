#include "FrequencySample.h"

template<typename FrameType>
speech::raw_data::FrequencySample<FrameType>::FrequencySample(int _size,
        std::shared_ptr<double> _amplitude, std::shared_ptr<double> _phase) {
    size = _size;
    amplitude = _amplitude;
    phase = _phase;
}

template<typename FrameType>
speech::raw_data::FrequencySample<FrameType>::~FrequencySample() {
    amplitude.reset();
    phase.reset();
}

template<typename FrameType>
int speech::raw_data::FrequencySample<FrameType>::getSize() {
    return size;
}

template<typename FrameType>
std::shared_ptr<double> speech::raw_data::FrequencySample<FrameType>::getAmplitude() {
    return amplitude;
}

template<typename FrameType>
std::shared_ptr<double> speech::raw_data::FrequencySample<FrameType>::getPhase() {
    return phase;
}

template
class speech::raw_data::FrequencySample<short>;

template
class speech::raw_data::FrequencySample<unsigned char>;