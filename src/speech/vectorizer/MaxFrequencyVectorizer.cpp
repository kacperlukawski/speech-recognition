#include "MaxFrequencyVectorizer.h"

#include <iostream>

template<typename FrameType>
speech::vectorizer::MaxFrequencyVectorizer<FrameType>::MaxFrequencyVectorizer(int n) {
    this->n = n;
    this->filterPtr = new MaxFrequenciesFilter<FrameType>(n);
}

template<typename FrameType>
speech::vectorizer::MaxFrequencyVectorizer<FrameType>::MaxFrequencyVectorizer(std::istream &in) {
    in.read((char *) &n, sizeof(n));

    this->filterPtr = new MaxFrequenciesFilter<FrameType>(n);
}

template<typename FrameType>
std::valarray<double> speech::vectorizer::MaxFrequencyVectorizer<FrameType>::vectorize(
        FrequencySample<FrameType> &sample) {
    FrequencySample<FrameType> filteredSample = filterPtr->filter(sample);

    std::valarray<double> vec(0.0, this->n);

    int currentPos = 0;
    double *amplitutePtr = filteredSample.getAmplitude().get();
    double *phasePtr = filteredSample.getPhase().get();
    for (int i = 1; i < filteredSample.getSize() / 2 + 1; i++) {
        if (currentPos >= this->n) {
            break;
        }

        if (amplitutePtr[i] > 0.0) {
            vec[currentPos++] = i; // sample.getIndexFrequency(i); // frequency of the local maximum
            vec[currentPos++] = amplitutePtr[i];             // amplitude of this maximum
        }
    }

    return vec;
}

template<typename FrameType>
void speech::vectorizer::MaxFrequencyVectorizer<FrameType>::serialize(std::ostream &out) const {
    uint32_t typeIdentifier = TYPE_IDENTIFIER;
    out.write((char const *) &typeIdentifier, sizeof(typeIdentifier));
    out.write((char const *) &n, sizeof(n));
}

template<typename FrameType>
int speech::vectorizer::MaxFrequencyVectorizer<FrameType>::getVectorSize() const {
    return this->n;
}

template
class speech::vectorizer::MaxFrequencyVectorizer<short int>;