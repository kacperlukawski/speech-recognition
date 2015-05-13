//
// Created by kacper on 10.05.15.
//

#include <algorithm>

#include "FormantVectorizer.h"

template<typename FrameType>
speech::vectorizer::FormantVectorizer<FrameType>::FormantVectorizer(std::istream &in) {
    // todo
}


template<typename FrameType>
speech::vectorizer::FormantVectorizer<FrameType>::FormantVectorizer() {
    // todo
}

//#include <iostream>

template<typename FrameType>
std::valarray<double> speech::vectorizer::FormantVectorizer<FrameType>::vectorize(FrequencySample<FrameType> &sample) {
    // F1 boundaries
    int pos300Hz = sample.getFrequencyIndex(300.0);
    int pos1000Hz = sample.getFrequencyIndex(1000.0);

    // F2 boundaries
    int pos850Hz = sample.getFrequencyIndex(850.0);
    int pos2500Hz = sample.getFrequencyIndex(2500.0);

    // F3 boundaries
    int pos2350Hz = sample.getFrequencyIndex(2350.0);
    int pos4000Hz = sample.getFrequencyIndex(4000.0);

    double *amplitudePtr = sample.getAmplitude().get();
    double *f1FormantPos = std::max_element(amplitudePtr + pos300Hz, amplitudePtr + pos1000Hz);
    double *f2FormantPos = std::max_element(amplitudePtr + pos850Hz, amplitudePtr + pos2500Hz);
    double *f3FormantPos = std::max_element(amplitudePtr + pos2350Hz, amplitudePtr + pos4000Hz);

    std::valarray<double> result(0.0, 4);
//    result[0] = *f1FormantPos;
//    result[1] = *f2FormantPos;
    result[1] = sample.getIndexFrequency(f1FormantPos - amplitudePtr);
    result[2] = sample.getIndexFrequency(f2FormantPos - amplitudePtr);
    result[3] = sample.getIndexFrequency(f3FormantPos - amplitudePtr);

//    std::cout << result[2] << " " << result[3] << std::endl;

    return result;
}

template<typename FrameType>
void speech::vectorizer::FormantVectorizer<FrameType>::serialize(std::ostream &out) const {
    uint32_t typeIdentifier = TYPE_IDENTIFIER;
    out.write((char const *) &typeIdentifier, sizeof(typeIdentifier));
}

template<typename FrameType>
int speech::vectorizer::FormantVectorizer<FrameType>::getVectorSize() const {
    return 4; // TODO: get real vector size
}

template
class speech::vectorizer::FormantVectorizer<short int>;
