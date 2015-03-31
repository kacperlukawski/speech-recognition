#include "MaxFrequenciesFilter.h"

#include <cmath>
#include <vector>
#include <algorithm>

// TODO: remove copy constructor of FrequencySample to avoid copying the values

template<typename FrameType>
speech::raw_data::filtering::MaxFrequenciesFilter<FrameType>::MaxFrequenciesFilter(int n) {
    this->n = n;
}

template<typename FrameType>
FrequencySample<FrameType> speech::raw_data::filtering::MaxFrequenciesFilter<FrameType>::filter(const FrequencySample<FrameType> &sample) {
    FrequencySample<FrameType> result(sample);
    double threshold = getNthMaxValue(result);

    double *amplitudePtr = result.getAmplitude().get();
    for (int i = 0; i < result.getSize(); i++) {
        if (fabs(amplitudePtr[i]) < threshold) {
            amplitudePtr[i] = 0.0;
            // TODO: it may be good to zero the phase
        }
    }

    return result;
}

template<typename FrameType>
double speech::raw_data::filtering::MaxFrequenciesFilter<FrameType>::getNthMaxValue(FrequencySample<FrameType> &sample) {
    std::vector<double> biggestElements;

    double *amplitudePtr = sample.getAmplitude().get();
    for (int i = 0; i < sample.getSize(); i++) {
        double value = fabs(amplitudePtr[i]);

        if (biggestElements.size() < 2 * n) { // FrequencySample has duplicates (see: Nyquist frequency)
            biggestElements.push_back(value);
            continue;
        }

        auto minPosition = std::min_element(biggestElements.begin(), biggestElements.end());
        if (value > *minPosition) {
            *minPosition = value;
        }
    }

    return *(std::min_element(biggestElements.begin(), biggestElements.end()));
}

template
class speech::raw_data::filtering::MaxFrequenciesFilter<short int>;