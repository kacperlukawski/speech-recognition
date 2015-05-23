//
// Created by kacper on 19.05.15.
//

#include <iostream>

#include "MFCCVectorizer.h"

template<typename FrameType>
speech::vectorizer::MFCCVectorizer<FrameType>::MFCCVectorizer(std::istream &in) {
    in.read((char *) &this->bins, sizeof(int));
    in.read((char *) &this->cepstralCoefficientsNumber, sizeof(int));
}

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

//        result[this->cepstralCoefficientsNumber] = result[0] + result[1]; // TODO: replace with energy

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
std::vector<std::valarray<double>> speech::vectorizer::MFCCVectorizer<FrameType>::vectorize(DataSource<FrameType> &dataSource) {
    std::vector<std::valarray<double>> results;
    for (auto it = dataSource.getSamplesIteratorBegin(); it != dataSource.getSamplesIteratorEnd(); it++) {
        DataSample<FrameType>& dataSample = *it;
        FrequencySample<FrameType> frequencySample = frequencyTransform->transform(dataSample);

        // creates empty vectors for sample
        std::valarray<double> result(0.0, getVectorSize());

        // calculates MFCC coefficients
        DataSample<FrameType> cepstrum = getCepstrum(frequencySample);
        FrameType *cepstrumValues = cepstrum.getValues().get();
        for (int i = 0; i < this->cepstralCoefficientsNumber; i++) {
            result[i] = *(cepstrumValues + i);
        }

        // calculates an energy of the samples (TODO: check if it works properly)
        FrameType *valuesBegin = dataSample.getValues().get();
        FrameType *valuesEnd = valuesBegin + dataSample.getSize();
        int sampleSize = dataSample.getSize();
        result[this->cepstralCoefficientsNumber] = 0.0;
        for (auto valueIt = valuesBegin; valueIt != valuesEnd; valueIt++) {
            result[this->cepstralCoefficientsNumber] += (double) *valueIt / sampleSize;
        }

//        std::cout << "result[this->cepstralCoefficientsNumber]: " << result[this->cepstralCoefficientsNumber] << std::endl;

        // stores the vector
        results.push_back(result);
    }


    int offset = this->cepstralCoefficientsNumber + 1;
    for (auto it = results.begin(); it != results.end(); it++) {
        // omits first and last vector, because deltas cannot be calculated
        if (it == results.begin() || (it + 1) == results.end()) {
            continue;
        }

        // gets vectors with results
        std::valarray<double>& result = results.at(it - results.begin());
        std::valarray<double>& prevResults = results.at(it - results.begin() - 1);
        std::valarray<double>& nextResults = results.at(it - results.begin() + 1);

        // calculates differences
        for (int i = 0; i < this->cepstralCoefficientsNumber + 1; i++) {
            result[offset + i] = (nextResults[i] - prevResults[i]) / 2.0;
        }
    }

    for (auto it = results.begin(); it != results.end(); it++) {
        // omits first and last vector, because deltas cannot be calculated
        if (it == results.begin() || (it + 1) == results.end()) {
            continue;
        }

        // gets vectors with results
        std::valarray<double>& result = results.at(it - results.begin());
        std::valarray<double>& prevResults = results.at(it - results.begin() - 1);
        std::valarray<double>& nextResults = results.at(it - results.begin() + 1);

        // calculates differences
        for (int i = 0; i < this->cepstralCoefficientsNumber + 1; i++) {
            result[2 * offset + i] = (nextResults[offset + i] - prevResults[offset + i]) / 2.0;
        }
    }

    return std::move(results);
}

template<typename FrameType>
void speech::vectorizer::MFCCVectorizer<FrameType>::serialize(std::ostream &out) const {
    uint32_t type = TYPE_IDENTIFIER;
    out.write((char const *) &type, sizeof(type));
    out.write((char const *) &this->bins, sizeof(int));
    out.write((char const *) &this->cepstralCoefficientsNumber, sizeof(int));
}

template<typename FrameType>
int speech::vectorizer::MFCCVectorizer<FrameType>::getVectorSize() const {
    return (this->cepstralCoefficientsNumber + 1) * 3;
}

template<typename FrameType>
DataSample<FrameType> speech::vectorizer::MFCCVectorizer<FrameType>::getCepstrum(
        const FrequencySample<FrameType> &sample) {
    double *amplitudePtr = sample.getAmplitude().get();

    double minFrequencyMel = herzToMel(64.0);   // 64Hz, can be sample.getMinFrequency() as well
    double maxFrequencyMel = herzToMel(8000.0); // 8kHz, can be sample.getMaxFrequency() as well
    double delta = (maxFrequencyMel - minFrequencyMel) / (this->bins + 1);

    int pos = 0;
    double filterPoints[this->bins + 1];
    for (double mels = minFrequencyMel; mels <= maxFrequencyMel; mels += delta, pos++) {
        filterPoints[pos] = melToHerz(mels);
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
