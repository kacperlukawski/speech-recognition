#include <iostream>
#include <cmath>

#include "ThirdsPowerVectorizer.h"

std::ostream &operator<<(std::ostream &out, std::valarray<double> vector);

template<typename FrameType>
const double speech::vectorizer::ThirdsPowerVectorizer<FrameType>::THIRDS_MIDDLE_FREQUENCIES[speech::vectorizer::ThirdsPowerVectorizer<FrameType>::THIRDS_MIDDLE_FREQUENCIES_COUNT] = {
        100, 125, 160, 200, 250, 315, 400, 500, 630, 800, 1000, 1250, 1600,
        2000, 2500, 3150, 4000, 5000, 6300, 8000, 10000, 12500, 16000, 20000
};

template<typename FrameType>
speech::vectorizer::ThirdsPowerVectorizer<FrameType>::ThirdsPowerVectorizer(std::istream &istream) {

}

template<typename FrameType>
std::valarray<double> speech::vectorizer::ThirdsPowerVectorizer<FrameType>::vectorize(
        FrequencySample<FrameType> &sample) {
    std::valarray<double> result(0.0, THIRDS_MIDDLE_FREQUENCIES_COUNT);

    double *amplitudePtr = sample.getAmplitude().get();
    for (int i = 0; i < THIRDS_MIDDLE_FREQUENCIES_COUNT; i++) {
        double middleFrequency = THIRDS_MIDDLE_FREQUENCIES[i];
        double lowerFrequency = middleFrequency / pow(2.0, 1.0 / 6.0);
        double upperFrequency = lowerFrequency * pow(2.0, 1.0 / 3.0);
        int lowerIndex = sample.getFrequencyIndex(lowerFrequency);
        int upperIndex = sample.getFrequencyIndex(upperFrequency);

        result[i] = getThirdEnergy(amplitudePtr, lowerIndex, upperIndex);
    }

    std::cout << result << std::endl;

    return result;
}

template<typename FrameType>
double speech::vectorizer::ThirdsPowerVectorizer<FrameType>::getThirdEnergy(double *amplitudePtr, int lowerIndex, int upperIndex) {
    double thirdEnergy = 0.0;

    for (int j = lowerIndex; j < upperIndex; j++) {
        thirdEnergy += pow(*(amplitudePtr + j), 2.0);
    }

//    thirdEnergy /= upperIndex - lowerIndex + 1;
    return thirdEnergy;
}

template<typename FrameType>
void speech::vectorizer::ThirdsPowerVectorizer<FrameType>::serialize(std::ostream &out) const {

}

template<typename FrameType>
int speech::vectorizer::ThirdsPowerVectorizer<FrameType>::getVectorSize() const {
    return THIRDS_MIDDLE_FREQUENCIES_COUNT;
}

template
class speech::vectorizer::ThirdsPowerVectorizer<short>;


//
// Overloads the << operator for std::valarray to have a simple method
// to display current values of vectors.
//
std::ostream &operator<<(std::ostream &out, std::valarray<double> vector) {
    for (int i = 0; i < vector.size(); i++) {
        out << vector[i] << " ";
    }

    return out;
}
