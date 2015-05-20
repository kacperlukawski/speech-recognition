//
// Created by kacper on 19.05.15.
//

#include <iostream>

#include "MFCCVectorizer.h"

template<typename FrameType>
std::valarray<double> speech::vectorizer::MFCCVectorizer<FrameType>::vectorize(FrequencySample<FrameType> &sample) {
    std::valarray<double> result(0.0, getVectorSize());

    DataSample<FrameType> cepstrum = getCepstrum(sample);
    FrameType *cepstrumValues = cepstrum.getValues().get();
    for (int i = 0; i < this->cepstralCoefficientsNumber; i++) {
        result[i] = *(cepstrumValues + i);
    }

    // TODO: check the implementation and add another features calculated from coefficients

    return result;
}

template<typename FrameType>
std::vector<std::valarray<double>> speech::vectorizer::MFCCVectorizer<FrameType>::vectorize(
        std::vector<FrequencySample<FrameType>> &samples) {
    std::vector<std::valarray<double>> results;
    for (auto it = samples.begin(); it != samples.end(); it++) {
        // creates empty vectors for sample
        std::valarray<double> result(0.0, getVectorSize());

        // calculates MFCC coefficients
        DataSample<FrameType> cepstrum = getCepstrum(*it);
        FrameType *cepstrumValues = cepstrum.getValues().get();
        for (int i = 0; i < this->cepstralCoefficientsNumber; i++) {
            result[i] = *(cepstrumValues + i);
        }

        // TODO: add calculating energy

        // stores the vector
        results.push_back(result);
    }

    int offset = this->cepstralCoefficientsNumber + 1;
    for (auto it = samples.begin(); it != samples.end(); it++) {
        // omits first and last vector, because deltas cannot be calculated
        if (it == samples.begin() || (it + 1) == samples.end()) {
            results.push_back(vectorize(*it));
            continue;
        }

        // gets vectors with results
        std::valarray<double>& result = results.at(it - samples.begin());
        std::valarray<double>& prevResults = results.at(it - samples.begin() - 1);
        std::valarray<double>& nextResults = results.at(it - samples.begin() + 1);

        // calculates differences
        for (int i = 0; i < this->cepstralCoefficientsNumber + 1; i++) {
            result[offset + i] = (nextResults[i] - prevResults[i]) / 2.0;
        }
    }

    for (auto it = samples.begin(); it != samples.end(); it++) {
        // omits first and last vector, because deltas cannot be calculated
        if (it == samples.begin() || (it + 1) == samples.end()) {
            results.push_back(vectorize(*it));
            continue;
        }

        // gets vectors with results
        std::valarray<double>& result = results.at(it - samples.begin());
        std::valarray<double>& prevResults = results.at(it - samples.begin() - 1);
        std::valarray<double>& nextResults = results.at(it - samples.begin() + 1);

        // calculates differences
        for (int i = 0; i < this->cepstralCoefficientsNumber + 1; i++) {
            result[2 * offset + i] = (nextResults[offset + i] - prevResults[offset + i]) / 2.0;
        }
    }

    return results;
}

template<typename FrameType>
void speech::vectorizer::MFCCVectorizer<FrameType>::serialize(std::ostream &out) const {

}

template<typename FrameType>
int speech::vectorizer::MFCCVectorizer<FrameType>::getVectorSize() const {
    return (this->cepstralCoefficientsNumber + 1) * 3;
}

template<typename FrameType>
DataSample<FrameType> speech::vectorizer::MFCCVectorizer<FrameType>::getCepstrum(
        const FrequencySample<FrameType> &sample) {
    double *amplitudePtr = sample.getAmplitude().get();

    double minFrequencyMel = getMelFrequency(64.0);   // 64Hz, can be sample.getMinFrequency() as well
    double maxFrequencyMel = getMelFrequency(8000.0); // 8kHz, can be sample.getMaxFrequency() as well
    double delta = (maxFrequencyMel - minFrequencyMel) / (this->bins + 1);

    int pos = 0;
    double filterPoints[this->bins + 1];
    for (double mels = minFrequencyMel; mels <= maxFrequencyMel; mels += delta, pos++) {
        filterPoints[pos] = getFrequencyFromMels(mels);
    }

    FrequencySample<FrameType> cepstrumFrequencySample(this->bins, 0,
                                                       std::shared_ptr<double>(new double[this->bins],
                                                                               std::default_delete<double[]>()),
                                                       std::shared_ptr<double>(new double[this->bins],
                                                                               std::default_delete<double[]>()));
    double *cepstralAmplitudePtr = cepstrumFrequencySample.getAmplitude().get();
    double *cepstralPhasePtr = cepstrumFrequencySample.getPhase().get();
    for (int i = 1; i <= this->bins; i++) {
        double lowerBoundary = filterPoints[i - 1];
        double higherBoundary = filterPoints[i + 1];
        double centerPoint = filterPoints[i];

        int lowerBoundaryIndex = sample.getFrequencyIndex(lowerBoundary);
        int higherBoundaryIndex = sample.getFrequencyIndex(higherBoundary);
        int centerPointIndex = sample.getFrequencyIndex(centerPoint);

        *(cepstralAmplitudePtr + i - 1) = 0;
        for (int j = lowerBoundaryIndex; j <= higherBoundaryIndex; j++) {
            double coeff;
            if (j <= centerPointIndex) {
                coeff = (double) (j - lowerBoundaryIndex) / (centerPointIndex - lowerBoundaryIndex);
            } else {
                coeff = (double) (higherBoundaryIndex - j) / (higherBoundaryIndex - centerPointIndex);
            }

            double amplitude = *(amplitudePtr + j);
            *(cepstralAmplitudePtr + i - 1) += amplitude * coeff;
        }

        *(cepstralAmplitudePtr + i - 1) = log(
                *(cepstralAmplitudePtr + i - 1) + 10e-16); // 10e-16 for smooting logarithm
        *(cepstralPhasePtr + i - 1) = 0.0; // zero the phase, it is probably necessary
    }

    return frequencyTransform->reverseTransform(cepstrumFrequencySample);
}

template
class speech::vectorizer::MFCCVectorizer<short>;
