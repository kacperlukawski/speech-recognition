#include "MaxFrequenciesFilter.h"

#include <map>
#include <cmath>
#include <vector>
#include <algorithm>

template<typename FrameType>
speech::raw_data::filtering::MaxFrequenciesFilter<FrameType>::MaxFrequenciesFilter(int n) {
    this->n = n;
}

template<typename FrameType>
FrequencySample<FrameType> speech::raw_data::filtering::MaxFrequenciesFilter<FrameType>::filter(const FrequencySample<FrameType> &sample) {
    FrequencySample<FrameType> result(sample);

    int sampleSize = result.getSize();

    double *amplitudePtr = result.getAmplitude().get();
    double *phasePtr = result.getPhase().get();

    std::map<int, double> maximas;
    for (int i = 1; i < (sampleSize / 2) - 1; i++) {
        if (*(amplitudePtr + i) > *(amplitudePtr + i - 1) && *(amplitudePtr + i) > *(amplitudePtr + i + 1)) {
            // this is a local maximum of the amplitude
            maximas.insert(std::pair<int, double>(i, *(amplitudePtr + i)));
        }
    }

    while (maximas.size() > n) {
        auto minPos = std::min_element(maximas.begin(),
                                       maximas.end(),
                                       [](std::map<int, double>::value_type& l,
                                          std::map<int, double>::value_type& r) -> bool {
                                           return l.second < r.second;
                                       });
        maximas.erase(minPos);
    }

    for (int i = 0; i < sampleSize / 2; i++) {
        if (maximas.find(i) == maximas.end()) {
            *(amplitudePtr + (sampleSize - i - 1)) = 0.0;
            *(phasePtr + (sampleSize - i - 1)) = 0.0;
            *(amplitudePtr + i) = 0.0;
            *(phasePtr + i) = 0.0;
            continue;
        }

        *(phasePtr + (sampleSize - i - 1)) = 0.0;
        *(phasePtr + i) = 0.0;
    }

    return result;
}

template
class speech::raw_data::filtering::MaxFrequenciesFilter<short int>;