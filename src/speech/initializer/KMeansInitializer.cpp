#include "KMeansInitializer.h"

void speech::initializer::KMeansInitializer::initialize(speech::HMMLexicon::MultivariateGaussianHMM &gaussianHMM) {
    unsigned int dimensionality = gaussianHMM.getDimensionality();
    unsigned int states = gaussianHMM.getStates();
    unsigned int mixtures = gaussianHMM.getNumberOfMixtures();
    std::vector<HMMLexicon::Observation> *utterances = gaussianHMM.getUtterances();

    unsigned int observationsNb = this->countObservations(utterances);

    // Calculates global variance of the data
    valarray<double> variance = this->calculateVariance(utterances, dimensionality, observationsNb);

    unsigned int nbUtterances = utterances->size();
    for (int n = 0; n < states; ++n) {
        HMMLexicon::GMMLikelihoodFunction &stateGMM = gaussianHMM.getHiddenState(n);

        std::vector<std::valarray<double>> vectors;
        for (auto it = utterances->begin(); it != utterances->end(); it++) {
            HMMLexicon::Observation &utterance = *it;
            int nbSamples = utterance.size();
            int rangeWidth = nbSamples / states;
            int beginIndex = n * rangeWidth;
            int endIndex = beginIndex + rangeWidth;
            for (int i = beginIndex; i < endIndex; i++) {
                vectors.push_back(utterance[i]);
            }
        }

        speech::clustering::KMeans kMeans(mixtures, dimensionality);
        kMeans.fit(vectors);

        std::vector<std::valarray<double>> *centroids = kMeans.getCentroids();
        for (int m = 0; m < mixtures; ++m) {
            stateGMM.setMeans(m, centroids->at(m));
            stateGMM.setVariances(m, variance);
        }
    }
}