//
// Created by kacper on 04.06.15.
//

#include "HMMLexicon.h"

speech::HMMLexicon::HMMLexicon(int dimensionality) : dimensionality(dimensionality) {

}

speech::HMMLexicon::~HMMLexicon() {
    for (auto it = unitModels.begin(); it != unitModels.end(); ++it) {
        delete it->second;
    }
}

void speech::HMMLexicon::addUtterance(const Observation &utterance,
                                      string transcription,
                                      string unitSeparator) {
    static unsigned int gaussians = 16; // number of Gaussians used for approximation

    if (unitModels.find(transcription) == unitModels.end()) {
        // given transcription was not passed before
        vector<string> substates = this->split(transcription, unitSeparator);
        int states = substates.size() + 1; // one additional state is for the silence at the end
        unitModels[transcription] = new MultivariateGaussianHMM(this->dimensionality, states, gaussians);
    }

    unitModels[transcription]->addUtterance(utterance);
}

string speech::HMMLexicon::predict(const Observation &observation) {
    // TODO: run Viterbi algorithm for all unit models and select the one with the highest probability (use OpenMP)
    return string("XXX");
}

void speech::HMMLexicon::fit() {
    // TODO: add OPENMP pragma
    for (auto it = this->unitModels.begin(); it != this->unitModels.end(); ++it) {
        it->second->fit();
    }
}

constexpr double speech::HMMLexicon::GMMLikelihoodFunction::MIN_VARIANCE;

speech::HMMLexicon::GMMLikelihoodFunction::GMMLikelihoodFunction(unsigned int M, unsigned int D)
        : M(M), D(D), weights(valarray<double>(1.0 / M, M)), means(vector<valarray<double>>(M)),
          variances(vector<valarray<double>>(M)) {
    // initialize means and variances
    for (int i = 0; i < this->M; i++) {
        means[i] = valarray<double>(0.0, D);
        variances[i] = valarray<double>(1.0, D);
    }
}

double speech::HMMLexicon::GMMLikelihoodFunction::operator()(const valarray<double> &observation) {
    if (observation.size() != this->D) {
        throw std::out_of_range("Given observation has wrong dimensionality");
    }

    double result = 0.0;
    for (int i = 0; i < this->M; i++) {
        valarray<double> difference = observation - this->means[i];
        valarray<double> invVariance = valarray<double>(1.0, D) / this->variances[i];
        double varianceNorm = invVariance.max(); // Euclidean norm of matrix is a highest eigenvalue,
        // but eigenvalues of the diagonal matrix are their non-zero values
        double power = ((difference / this->variances[i]) * difference).sum();
        result += this->weights[i] * exp(-0.5 * power) / (pow(2 * M_PI, this->D / 2.0) * varianceNorm);
    }

    return result;
}

speech::HMMLexicon::MultivariateGaussianHMM::MultivariateGaussianHMM(unsigned int dimensionality, unsigned int states,
                                                                     unsigned int M)
        : dimensionality(dimensionality), states(states), M(M) {
    this->utterances = new vector<Observation>();
    this->hiddenStates = new vector<GMMLikelihoodFunction>();
    this->pi = new double[states];
    this->transition = new double *[states];

    for (int i = 0; i < states; i++) {
        this->transition[i] = new double[states];
        initTransitionsRandomly(this->transition[i], states);

        this->hiddenStates->push_back(
                GMMLikelihoodFunction(M, dimensionality)); // assign a multivariate Gaussian for each state
        this->pi[i] = 1.0 / states; // assign start probability of each state
    }
}

speech::HMMLexicon::MultivariateGaussianHMM::~MultivariateGaussianHMM() {
    delete this->utterances;
    delete this->hiddenStates;
    delete[] this->pi;
    delete[] this->transition;
}

void speech::HMMLexicon::MultivariateGaussianHMM::addUtterance(const Observation &utterance) {
    utterances->push_back(utterance);
}

void speech::HMMLexicon::MultivariateGaussianHMM::fit() {
    // TODO: set means and variances of the Gaussians using some data coming from the utterances

    // for each utterance
    for (auto utteranceIt = this->utterances->begin(); utteranceIt != this->utterances->end(); ++utteranceIt) {
        // calculate forward and backward probabilities
        int times = utteranceIt->size();
        int states = this->states;
        double **forward = new double *[times];
        double **backward = new double *[times];
        double **occupation = new double *[times];
        for (int i = 0; i < times; i++) {
            forward[i] = new double[states];
            backward[i] = new double[states];
            occupation[i] = new double[states];
        }

        // initialize the forward probabilities
        forward[0][0] = 1.0;
        for (int i = 1; i < this->states; i++) {
            forward[0][i] = 0.0;
        }

        // initialize the backward probabilities
        int T = times - 1;
        int E = states - 1;
        for (int i = 0; i < this->states; i++) {
            backward[T][i] = this->transition[i][E];
        }

        // recursively calculate the forward probabilities for each time-state pairs
        for (int j = 0; j < states; j++) {
            for (int t = 1; t < times; t++) {
                forward[t][j] = 0.0; // initialize forward probability of being in state j in time t
                for (int i = 0; i < states; i++) {
                    forward[t][j] += forward[t - 1][i] * this->transition[i][j];
                }
                double emission = this->hiddenStates->at(j)(utteranceIt->at(t));
                forward[t][j] *= emission; // multiply by emission probability
            }
        }

        // recursively calculate the backward probabilites
        for (int i = 0; i < states; i++) {
            for (int t = T - 1; t >= 0; t--) {
                backward[t][i] = 0.0;
                for (int j = 0; j < states; j++) {
                    backward[t][i] += this->transition[i][j] * backward[t + 1][j] *
                                      this->hiddenStates->at(j)(utteranceIt->at(t + 1));
                }
            }
        }

        // calculate the state occupation probability
        for (int t = 0; t < times; t++) {
            for (int j = 0; j < states; j++) {
                occupation[t][j] = forward[t][j] * backward[t][j];
            }
        }

        std::cout << "Forward:" << std::endl;
        for (int i = 0; i < times; i++) {
            for (int j = 0; j < states; j++) {
                std::cout << forward[i][j] << ' ';
            }

            std::cout << std::endl;
        }
        std::cout << std::endl;

        std::cout << "Backward:" << std::endl;
        for (int i = 0; i < times; i++) {
            for (int j = 0; j < states; j++) {
                std::cout << backward[i][j] << ' ';
            }

            std::cout << std::endl;
        }
        std::cout << std::endl;

        std::cout << "Occupation:" << std::endl;
        for (int i = 0; i < times; i++) {
            for (int j = 0; j < states; j++) {
                std::cout << occupation[i][j] << ' ';
            }

            std::cout << std::endl;
        }
        std::cout << std::endl;

        std::cout << "Transition:" << std::endl;
        for (int i = 0; i < states; i++) {
            for (int j = 0; j < states; j++) {
                std::cout << this->transition[i][j] << ' ';
            }

            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
    // TODO: implement the Bauch-Welch algortihm for learning the coefficients

    // TODO: add the end all utterances can be removed, and model can be marked as fitted
}