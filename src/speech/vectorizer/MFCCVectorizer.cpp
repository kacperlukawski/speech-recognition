//
// Created by kacper on 19.05.15.
//

#include <iostream>

#include "MFCCVectorizer.h"

template<typename FrameType>
speech::vectorizer::MFCCVectorizer<FrameType>::MFCCVectorizer(std::istream &in) {
    in.read((char *) &this->bins, sizeof(int));
    in.read((char *) &this->cepstralCoefficientsNumber, sizeof(int));
    in.read((char *) &this->minCepstrumFrequency, sizeof(double));
    in.read((char *) &this->maxCepstrumFrequency, sizeof(double));
    buildFilterBank();
}

template<typename FrameType>
std::valarray<double> speech::vectorizer::MFCCVectorizer<FrameType>::vectorize(FrequencySample<FrameType> &sample) {
    std::valarray<double> result(0.0, getVectorSize());

    std::valarray<double> cepstrum = calculateCepstrumCoefficients(sample);
    for (int i = 0; i < this->cepstralCoefficientsNumber; i++) {
        result[i] = cepstrum[i];
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
        std::valarray<double> cepstrum = calculateCepstrumCoefficients(*it);
        for (int i = 0; i < this->cepstralCoefficientsNumber; i++) {
            result[i] = cepstrum[i];
        }

        Spectrum spectrum(*it);
        double energy = spectrum.getValues().sum();
        result[this->cepstralCoefficientsNumber] = energy;

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
    int vectorSize = this->getVectorSize();
    std::vector<std::valarray<double>> results;
    for (auto it = dataSource.getSamplesIteratorBegin(); it != dataSource.getSamplesIteratorEnd(); it++) {
        DataSample<FrameType>& dataSample = *it;
        FrequencySample<FrameType> frequencySample = frequencyTransform->transform(dataSample);

        // creates empty vectors for sample
        std::valarray<double> result(0.0, vectorSize);

        // calculates MFCC coefficients
        std::valarray<double> cepstrum = calculateCepstrumCoefficients(frequencySample);
        for (int i = 0; i < this->cepstralCoefficientsNumber; i++) {
            result[i] = cepstrum[i]; // TODO: check if there should be logarithm here
        }

        Spectrum spectrum(frequencySample);
        double energy = spectrum.getValues().sum();
        result[this->cepstralCoefficientsNumber] = energy;

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
    out.write((char const *) &this->minCepstrumFrequency, sizeof(double));
    out.write((char const *) &this->maxCepstrumFrequency, sizeof(double));
}

template<typename FrameType>
int speech::vectorizer::MFCCVectorizer<FrameType>::getVectorSize() const {
    return (this->cepstralCoefficientsNumber + 1) * 3;
}

template<typename FrameType>
std::valarray<double> speech::vectorizer::MFCCVectorizer<FrameType>::calculateCepstrumCoefficients( // TODO: do not use DataSample, it is just a vector of features!
        const FrequencySample<FrameType> &sample) {
    // spectrum of the sample
    Spectrum spectrum(sample);

    // container for log energies of each filter
    std::valarray<double> logEnergies(0.0, this->bins);

    int position = 0;
    for (auto filterIt = this->filterBank.begin(); filterIt != this->filterBank.end(); filterIt++) {
        // calculate the log of energy in this particular filter
        logEnergies[position] = log((*filterIt)(spectrum));
        position++;
    }

    // container for final cepstral coefficients
    std::valarray<double> cepstralCoefficients(this->cepstralCoefficientsNumber);
    for (int i = 0; i < this->cepstralCoefficientsNumber; i++) {
        double coefficient = 0.0;
        for (int j = 0; j < this->bins; j++) {
            coefficient += logEnergies[j] * cos((M_PI * i) / this->bins * (j + 0.5));
        }
        cepstralCoefficients[i] = coefficient;
    }

//    std::cout << "Cepstral coefficients: ";
//    for (int i = 0; i < this->cepstralCoefficientsNumber; i++) {
//        std::cout << cepstralCoefficients[i] << " ";
//    }

    return std::move(cepstralCoefficients);
}


template<typename FrameType>
void speech::vectorizer::MFCCVectorizer<FrameType>::buildFilterBank() {
    double minFrequencyMel = herzToMel(this->minCepstrumFrequency);
    double maxFrequencyMel = herzToMel(this->maxCepstrumFrequency);
    double delta = (maxFrequencyMel - minFrequencyMel) / (this->bins + 1);

    int boundaries = this->bins + 2;
    double filterPoints[boundaries];
    for (int i = 0; i < boundaries; i++) {
        filterPoints[i] = melToHerz(minFrequencyMel + i * delta);
    }

    for (int i = 1; i < this->bins + 1; i++) {
        this->filterBank.push_back(MelFilter(filterPoints[i - 1], filterPoints[i + 1]));
    }
}

template
class speech::vectorizer::MFCCVectorizer<short>;
