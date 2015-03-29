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
    for (int i = 0; i < filteredSample.getSize() / 2 + 1; i++) {
        double value = amplitutePtr[i];
        if (value > 0.0) {
            result.push_back((double) i);
            if (value > 1.0) {
                result.push_back(value);
            } else {
                result.push_back(0.0);
            }
        }
    }

    return result;
}

template
class speech::vectorizer::MaxFrequencyVectorizer<short int>;