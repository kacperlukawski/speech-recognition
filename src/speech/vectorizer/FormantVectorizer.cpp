//
// Created by kacper on 10.05.15.
//

#include "FormantVectorizer.h"

template<typename FrameType>
speech::vectorizer::FormantVectorizer<FrameType>::FormantVectorizer(std::istream &in) {
    // todo
}

template<typename FrameType>
std::valarray<double> speech::vectorizer::FormantVectorizer<FrameType>::vectorize(FrequencySample<FrameType> &sample) {
    // todo
    return std::valarray<double>();
}

template<typename FrameType>
void speech::vectorizer::FormantVectorizer<FrameType>::serialize(std::ostream &out) const {
    uint32_t typeIdentifier = TYPE_IDENTIFIER;
    out.write((char const *) &typeIdentifier, sizeof(typeIdentifier));
}

template
class speech::vectorizer::FormantVectorizer<short int>;