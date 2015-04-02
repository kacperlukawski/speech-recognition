#include "MaxFrequencyVectorizer.h"

template<typename FrameType>
speech::vectorizer::MaxFrequencyVectorizer<FrameType>::MaxFrequencyVectorizer(int n) {
    this->filterPtr = new MaxFrequenciesFilter<FrameType>(n);
}

template<typename FrameType>
std::vector<double> speech::vectorizer::MaxFrequencyVectorizer<FrameType>::vectorize(FrequencySample<FrameType> &sample) {
    FrequencySample<FrameType> filteredSample = filterPtr->filter(sample);

    std::vector<double> result;

    double *amplitutePtr = filteredSample.getAmplitude().get();
    double *phasePtr = filteredSample.getPhase().get();
    for (int i = 0; i < filteredSample.getSize() / 2 + 1; i++) {
        double amplitude = amplitutePtr[i];
        double phase = phasePtr[i];
        if (amplitude > 0.0) {
            result.push_back((double) i);
            result.push_back(amplitude);
//            result.push_back(phase);
        }
    }

    return result;
}

template
class speech::vectorizer::MaxFrequencyVectorizer<short int>;