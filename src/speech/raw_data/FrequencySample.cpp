#include "FrequencySample.h"

#include <cstring>

template<typename FrameType>
speech::raw_data::FrequencySample<FrameType>::FrequencySample(int _size, int _length,
                                                              std::shared_ptr<double> _amplitude,
                                                              std::shared_ptr<double> _phase) {
    size = _size;
    length = _length;
    amplitude = _amplitude;
    phase = _phase;
}

template<typename FrameType>
speech::raw_data::FrequencySample<FrameType>::FrequencySample(const FrequencySample<FrameType> &ref) {
    size = ref.size;
    length = ref.length;
    amplitude = std::shared_ptr<double>(new double[size], std::default_delete<double[]>());
    phase = std::shared_ptr<double>(new double[size], std::default_delete<double[]>());

    memcpy(amplitude.get(), ref.amplitude.get(), size * sizeof(double));
    memcpy(phase.get(), ref.phase.get(), size * sizeof(double));
}

template<typename FrameType>
speech::raw_data::FrequencySample<FrameType>::~FrequencySample() {
    amplitude.reset();
    phase.reset();
}

template<typename FrameType>
int speech::raw_data::FrequencySample<FrameType>::getSize() const {
    return size;
}

template<typename FrameType>
int speech::raw_data::FrequencySample<FrameType>::getLength() const {
    return length;
}

template<typename FrameType>
std::shared_ptr<double> speech::raw_data::FrequencySample<FrameType>::getAmplitude() const {
    return amplitude;
}

template<typename FrameType>
std::shared_ptr<double> speech::raw_data::FrequencySample<FrameType>::getPhase() const {
    return phase;
}

template
class speech::raw_data::FrequencySample<short>;

template
class speech::raw_data::FrequencySample<unsigned char>;
