#include "HarmonicsVectorizer.h"

#include <set>

using std::set;

template<typename FrameType>
std::valarray<double> speech::vectorizer::HarmonicsVectorizer<FrameType>::vectorize(FrequencySample<FrameType> &sample) {
    std::valarray<double> result(maxHarmonics);

    // TODO: correct the algorithm

    int minFrequencyIndex = sample.getFrequencyIndex(sample.getMinFrequency());
    int maxFrequencyIndex = sample.getFrequencyIndex(sample.getMaxFrequency());

    int sampleSize = sample.getSize();
    double *amplitudePtr = sample.getAmplitude().get();
    double maxAmplitude = *(std::max_element(amplitudePtr + minFrequencyIndex,
                                             amplitudePtr + maxFrequencyIndex));

    int foundHarmonics = 0;
    set<int> harmonicsIndexes;
    while (foundHarmonics < maxHarmonics && maxAmplitude > EPS) {
        for (int i = minFrequencyIndex; i < maxFrequencyIndex && foundHarmonics < maxHarmonics; i++) {
            if (*(amplitudePtr + i) < maxAmplitude) {
                // the value of current position is lower than maxAmplitude
                continue;
            }

            if (harmonicsIndexes.find(i) != harmonicsIndexes.end()) {
                // this value has been already used
                continue;
            }

            harmonicsIndexes.insert(i);

            result[foundHarmonics] = sample.getIndexFrequency(i);
            foundHarmonics++;
        }

        maxAmplitude -= 1;
    }

    return result;
}

template<typename FrameType>
void speech::vectorizer::HarmonicsVectorizer<FrameType>::serialize(std::ostream &out) const {
    // TODO: implement
}

template<typename FrameType>
int speech::vectorizer::HarmonicsVectorizer<FrameType>::getVectorSize() const {
    return maxHarmonics;
}

template
class speech::vectorizer::HarmonicsVectorizer<short>;