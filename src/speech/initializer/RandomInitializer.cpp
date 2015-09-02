//
// Created by kacper on 29.08.15.
//

#include "RandomInitializer.h"

void speech::initializer::RandomInitializer::initialize(speech::HMMLexicon::MultivariateGaussianHMM &gaussianHMM) {
    unsigned int dimensionality = gaussianHMM.getDimensionality();
    unsigned int states = gaussianHMM.getStates();
    unsigned int mixtures = gaussianHMM.getNumberOfMixtures();
    std::vector<HMMLexicon::Observation> *utterances = gaussianHMM.getUtterances();

    unsigned int observationsNb = this->countObservations(utterances);

    // Calculates global variance of the data
    valarray<double> variance = this->calculateVariance(utterances, dimensionality, observationsNb);

    // Divide each utterance to have N parts, where N is a number of hidden states
    // Select M random vectors to be the initial means of the mixtures
    srand(time(NULL));
    unsigned int nbUtterances = utterances->size();
    for (int n = 0; n < states; ++n) {
        HMMLexicon::GMMLikelihoodFunction &stateGMM = gaussianHMM.getHiddenState(n);
        for (int m = 0; m < mixtures; ++m) {
            int utteranceIdx = random() % nbUtterances;
            HMMLexicon::Observation &utterance = utterances->at(utteranceIdx);
            int nbSamples = utterance.size();
            int rangeWidth = nbSamples / states;
            int beginIndex = n * rangeWidth;
            int randomStateIndex = rand() % rangeWidth + beginIndex; // random vector
            stateGMM.setMeans(m, utterance[randomStateIndex]); // set means of the mixture to randomly chosen vector
            stateGMM.setVariances(m, variance);
        }
    }
}

unsigned int speech::initializer::RandomInitializer::countObservations(
        std::vector<HMMLexicon::Observation> *utterances) {
    unsigned int observationsNb = 0;
    for (auto it = utterances->begin(); it != utterances->end(); ++it) {
        HMMLexicon::Observation &observation = *it;
        observationsNb += observation.size();
    }

    return observationsNb;
}

std::valarray<double> speech::initializer::RandomInitializer::calculateMean(
        vector<speech::HMMLexicon::Observation> *utterances, int dimensionality, int observationsNb) {
    valarray<double> mean(0.0, dimensionality);
    for (auto it = utterances->begin(); it != utterances->end(); ++it) {
        HMMLexicon::Observation &observation = *it;
        for (auto vectorIt = observation.begin(); vectorIt != observation.end(); ++vectorIt) {
            valarray<double> &vector = *vectorIt;
            mean += vector / (double) observationsNb;
        }
    }

    return std::move(mean);
}

std::valarray<double> speech::initializer::RandomInitializer::calculateVariance(
        vector<speech::HMMLexicon::Observation> *utterances, int dimensionality,
        int observationsNb) {
    valarray<double> mean = this->calculateMean(utterances, dimensionality, observationsNb);
    valarray<double> variance(0.0, dimensionality);
    for (auto it = utterances->begin(); it != utterances->end(); ++it) {
        HMMLexicon::Observation &observation = *it;
        for (auto vectorIt = observation.begin(); vectorIt != observation.end(); ++vectorIt) {
            valarray<double> &vector = *vectorIt;
            valarray<double> difference = vector - mean;
            valarray<double> product = difference * difference;
            variance += product / (double) observationsNb;
        }
    }

    return std::move(variance);
}
