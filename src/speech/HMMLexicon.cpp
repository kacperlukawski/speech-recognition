//
// Created by kacper on 04.06.15.
//

#include "HMMLexicon.h"
#include "helpers.h"

speech::HMMLexicon::HMMLexicon(int dimensionality, unsigned int gaussians)
        : dimensionality(dimensionality), gaussians(gaussians) {

}

speech::HMMLexicon::~HMMLexicon() {
    for (auto it = unitModels.begin(); it != unitModels.end(); ++it) {
        delete it->second;
    }
}

void speech::HMMLexicon::addUtterance(const Observation &utterance,
                                      string transcription,
                                      string unitSeparator) {
    if (unitModels.find(transcription) == unitModels.end()) {
        // given transcription was not passed before
        vector<string> substates = this->split(transcription, unitSeparator);
        int states = substates.size() + 1; // one additional state is for the end state
        unitModels[transcription] = new MultivariateGaussianHMM(this->dimensionality, states, gaussians);
    }

    unitModels[transcription]->addUtterance(utterance);
}

string speech::HMMLexicon::predict(const Observation &observation) {
    std::map<string, double> modelLikelihood;
    for (auto it = this->unitModels.begin(); it != this->unitModels.end(); ++it) {
        double likelihood = it->second->predict(observation);
        modelLikelihood[it->first] = likelihood;
    }

    string bestLabel;
    double maxLikelihood = -INFINITY;
    for (auto it = modelLikelihood.begin(); it != modelLikelihood.end(); ++it) {
        std::cout << "PREDICTED: " << it->first << " -> " << it->second << std::endl;

        if (it->second > maxLikelihood) {
            maxLikelihood = it->second;
            bestLabel = it->first;
        }
    }

    return bestLabel;
}

void speech::HMMLexicon::fit() {
#ifdef _OPENMP
#pragma openmp parallel for
#endif
    for (auto it = this->unitModels.begin(); it != this->unitModels.end(); ++it) {
        std::cout << "Fitting model: " << it->first << std::endl;
        it->second->fit();
    }
}

constexpr double speech::HMMLexicon::GMMLikelihoodFunction::MIN_VARIANCE;

speech::HMMLexicon::GMMLikelihoodFunction::GMMLikelihoodFunction(unsigned int M, unsigned int D)
        : M(M), D(D), weights(valarray<double>(1.0 / M, M)), means(vector<valarray<double >>(M)),
          variances(vector<valarray<double >>(M)) {
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
        double varianceNorm = invVariance.max(); // Euclidean norm of matrix is a highest eigenvalue, but eigenvalues of the diagonal matrix are their non-zero values
        double power = ((difference / this->variances[i]) * difference).sum();
        result += this->weights[i] * exp(-0.5 * power) / (pow(2 * M_PI, this->D / 2.0) * varianceNorm);
    }

    return result;
}

double speech::HMMLexicon::GMMLikelihoodFunction::operator()(unsigned int k, const valarray<double> &observation) {
    if (k >= this->M) {
        throw std::out_of_range("Trying to use non-existing k-th mixture");
    }

    if (observation.size() != this->D) {
        throw std::out_of_range("Given observation has wrong dimensionality");
    }

    valarray<double> difference = observation - this->means[k];
    valarray<double> invVariance = valarray<double>(1.0, D) / this->variances[k];
    double varianceNorm = invVariance.max();
    double power = ((difference / this->variances[k]) * difference).sum();
    return this->weights[k] * exp(-0.5 * power) / (pow(2 * M_PI, this->D / 2.0) * varianceNorm);
}

constexpr double speech::HMMLexicon::MultivariateGaussianHMM::EPS;
constexpr double speech::HMMLexicon::MultivariateGaussianHMM::MIN_VARIANCE;

speech::HMMLexicon::MultivariateGaussianHMM::MultivariateGaussianHMM(unsigned int dimensionality, unsigned int states,
                                                                     unsigned int M)
        : dimensionality(dimensionality), states(states), M(M) {
    this->utterances = new vector<Observation>();
    this->hiddenStates = new vector<GMMLikelihoodFunction>();
    this->pi = new double[states];
    this->transition = new double *[states];

    for (int i = 0; i < states; i++) {
        this->hiddenStates->push_back(GMMLikelihoodFunction(M, dimensionality));
        this->transition[i] = new double[states];
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
    // 1. Initialize the mixtures in a random way, but using the data provided.
    // 2. For each utterance (r = 1,...,R) do:
    //    2.1. Compute forward likelihoods (alpha)
    //    2.2. Compute backward likelihoods (beta)
    //    2.3. Compute mixture occupation probabilities estimations (gamma)
    //    2.4. Compute transition probabilities estimations (ksi)
    //    2.5. Accumulate initial probabilities estimations (pi), transition probabilities estimations (a_ij),
    //         state occupation probabilities estimations (a_i), mixtures means estimations (mi_im), mixtures
    //         variances estimations (sigma_im), mixtures occupation probabilties estimations (b_im)
    // 3. Update the models using data accumulated from all utterances

    this->initializeMixtures();
    this->initializeModel();

    double logLikelihood = -INFINITY;
    double lastLogLikelihood = 0.0;
    unsigned long int iteration = 0;
    while (logLikelihood != lastLogLikelihood && iteration < MAX_ITERATIONS) {
        // Reset values for the next iteration
        lastLogLikelihood = logLikelihood;
        logLikelihood = 0.0;
        iteration++;

        // Create temporary structures used to accumulate data between iterations
        double *initialAcc = new double[this->states];
        double **transitionsAcc = new double *[this->states];
        double **occupationsAcc = new double *[this->states]; // [state][mixture]
        valarray<double> **weightedObservationAcc = new valarray<double> *[this->states];
        valarray<double> **weightedVarianceAcc = new valarray<double> *[this->states];
        for (int s1 = 0; s1 < this->states; ++s1) {
            initialAcc[s1] = 0.0;
            transitionsAcc[s1] = new double[this->states];
            for (int s2 = 0; s2 < this->states; ++s2) {
                transitionsAcc[s1][s2] = 0.0;
            }
            occupationsAcc[s1] = new double[this->M];
            for (int m = 0; m < this->M; ++m) {
                occupationsAcc[s1][m] = 0.0;
            }
            weightedObservationAcc[s1] = new valarray<double>[this->M];
            for (int m = 0; m < this->M; ++m) {
                weightedObservationAcc[s1][m] = valarray<double>(0.0, this->dimensionality);
            }
            weightedVarianceAcc[s1] = new valarray<double>[this->M];
            for (int m = 0; m < this->M; ++m) {
                weightedVarianceAcc[s1][m] = valarray<double>(MIN_VARIANCE, this->dimensionality);
            }
        }

        unsigned int utterancesNb = this->utterances->size();
        for (unsigned int r = 0; r < utterancesNb; ++r) {
            Observation &observation = this->utterances->at(r);
            unsigned int vectorsNb = observation.size();
            if (vectorsNb == 0) {
                continue;
            }

            // Create a structures necessary for the forward and backward estimates
            double **forward = new double *[this->states];
            double **backward = new double *[this->states];
            for (int s = 0; s < this->states; s++) {
                forward[s] = new double[vectorsNb]; // alpha_t(i) -> forward[i][t]
                backward[s] = new double[vectorsNb]; // beta_t(i) -> backward[i][t]
            }

            // Calculate the forward estimates for the first time frame
            valarray<double> &firstVector = observation[0];
            for (int i = 0; i < this->states; ++i) {
                GMMLikelihoodFunction &stateGMM = this->hiddenStates->at(i);
                double stateOccupation = stateGMM(firstVector);
                forward[i][0] = this->pi[i] * stateOccupation;
            }

            unsigned int T = vectorsNb - 1;
            // Calculate the forward estimates for all other time frames
            for (int t = 1; t <= T; ++t) {
                valarray<double> &vector = observation[t];
                for (int j = 0; j < this->states; j++) {
                    GMMLikelihoodFunction &stateGMM = this->hiddenStates->at(j);
                    double stateOccupation = stateGMM(vector); // stateOccupation is sometimes 0

                    double sum = 0.0;
                    for (int i = 0; i < this->states; i++) {
                        sum += forward[i][t - 1] * this->transition[i][j];
                    }

                    forward[j][t] = sum * stateOccupation + EPS;
                }
            }

            // Calculate the backward estimates for the last time frame
            for (int i = 0; i < this->states; i++) {
                backward[i][T] = 1.0;
            }

            // Calculate the backward estimates for all other time frames
            for (int t = T - 1; t >= 0; --t) {
                valarray<double> &nextVector = observation[t + 1];
                for (int i = 0; i < this->states; ++i) {
                    backward[i][t] = EPS;
                    for (int j = 0; j < this->states; ++j) {
                        GMMLikelihoodFunction &stateGMM = this->hiddenStates->at(j);
                        double stateOccupation = stateGMM(nextVector);
                        backward[i][t] += this->transition[i][j] * stateOccupation * backward[j][t + 1];
                    }
                }
            }

            // Create a structure necessary for occupation estimates
            double ***occupation = new double **[this->states];
            for (int s = 0; s < this->states; ++s) {
                occupation[s] = new double *[this->M];
                for (int m = 0; m < this->M; ++m) {
                    occupation[s][m] = new double[vectorsNb]; // gamma_t(i, m) -> occupation[i][m][t]
                }
            }

            // Fill the state occupation estimates
            for (int j = 0; j < this->states; ++j) {
                GMMLikelihoodFunction &stateGMM = this->hiddenStates->at(j);
                for (int t = 0; t <= T; ++t) {
                    double denominator = EPS;
                    for (int i = 0; i < this->states; ++i) {
                        denominator += forward[i][t] * backward[i][t];
                    }

                    valarray<double> &vector = observation[t];
                    double vectorProbability = stateGMM(vector) + EPS;
                    double globalOccupation = forward[j][t] * backward[j][t] / denominator;
                    double alternative = exp(log(forward[j][t]) + log(backward[j][t]) - log(denominator));

                    for (int m = 0; m < this->M; ++m) {
                        if (vectorProbability == 0.0) {
                            // If the vector probability is equal to zero, then set the probability manually to
                            // zero, in order to avoid having NaN due to division by zero.
                            occupation[j][m][t] = 0.0;
                            continue;
                        }

                        double vectorProbabilityInMixture = stateGMM(m, vector);
                        occupation[j][m][t] = globalOccupation * (vectorProbabilityInMixture / vectorProbability);
                    }
                }
            }

            // Create a structure necessary for transition estimates
            double ***transitions = new double **[this->states];
            for (int s1 = 0; s1 < this->states; ++s1) {
                transitions[s1] = new double *[this->states];
                for (int s2 = 0; s2 < this->states; ++s2) {
                    transitions[s1][s2] = new double[vectorsNb]; // ksi_t(i, j) -> transitions[i][j][t]
                }
            }

            // Calculate the transition estimates
            for (int i = 0; i < this->states; ++i) {
                for (int j = 0; j < this->states; ++j) {
                    GMMLikelihoodFunction &stateGMM = this->hiddenStates->at(j);
                    for (int t = 0; t < T; ++t) {
                        double globalOccupation = 0.0;
                        for (int m = 0; m < this->M; ++m) {
                            globalOccupation += occupation[i][m][t];
                        }

                        valarray<double> &nextVector = observation[t + 1];
                        double nextVectorProbability = stateGMM(nextVector);

                        double alternativeDenominator = 0.0;
                        for (int k = 0; k < this->states; ++k) {
                            for (int l = 0; l < this->states; ++l) {
                                GMMLikelihoodFunction &nextStateGMM = this->hiddenStates->at(l);
                                double nextStateProbabilityOfVector = nextStateGMM(nextVector);
                                alternativeDenominator +=
                                        forward[k][t] * this->transition[k][l] * nextStateProbabilityOfVector *
                                        backward[l][t + 1];
                            }
                        }

                        double transition =
                                globalOccupation * nextVectorProbability * this->transition[i][j] * backward[j][t + 1] /
                                backward[i][t];

                        if (alternativeDenominator == 0.0) {
                            // If the global occupation is equal to zero, then set the transition probability to zero,
                            // in order to avoid having NaNs due to division by zero.`
                            transitions[i][j][t] = 0.0;
                            continue;
                        }

                        double alternativeTransition =
                                forward[i][t] * this->transition[i][j] * nextVectorProbability * backward[j][t + 1] /
                                alternativeDenominator;

                        double test = exp(
                                log(globalOccupation) + log(nextVectorProbability) + log(this->transition[i][j]) +
                                log(backward[j][t + 1]) - log(backward[i][t]));
                        transitions[i][j][t] = alternativeTransition; // was transition before
                    }
                }
            }

            // Accumulate initial probabilities
            for (int s = 0; s < this->states; ++s) {
                double sum = 0.0;
                for (int m = 0; m < this->M; ++m) {
                    sum += occupation[s][m][0];
                }

                initialAcc[s] += sum;
            }

            // Accumulate transitions
            for (int s1 = 0; s1 < this->states; ++s1) {
                for (int s2 = 0; s2 < this->states; ++s2) {
                    double sum = 0.0;
                    for (int t = 0; t < T; ++t) {
                        sum += transitions[s1][s2][t];
                    }

                    transitionsAcc[s1][s2] += sum;
                }
            }

            // Accumulate mixtures occupations
            for (int s = 0; s < this->states; ++s) {
                GMMLikelihoodFunction &stateGMM = this->hiddenStates->at(s);
                for (int m = 0; m < this->M; ++m) {
                    double sum = 0.0;
                    const valarray<double> &mixtureMean = stateGMM.getMeans(m);
                    for (int t = 0; t < vectorsNb; ++t) {
                        if (isnan(occupation[s][m][t])) {
                            occupation[s][m][t] = 0.0;
                        }

                        sum += occupation[s][m][t];
                        weightedObservationAcc[s][m] += occupation[s][m][t] * observation[t];

                        valarray<double> difference = observation[t] - mixtureMean;
                        valarray<double> product = difference * difference;
                        weightedVarianceAcc[s][m] += occupation[s][m][t] * product;
                    }

                    occupationsAcc[s][m] += sum;
                }
            }

            // Accumulate logprob
            double sum = EPS;
            for (int s = 0; s < this->states; s++) {
                sum += forward[s][T];
            }
            logLikelihood += log(sum);

            // Remove the temporary structure for transition estimates
            for (int s1 = 0; s1 < this->states; ++s1) {
                for (int s2 = 0; s2 < this->states; ++s2) {
                    delete[] transitions[s1][s2];
                }
                delete[] transitions[s1];
            }
            delete[] transitions;

            // Remove the temporary structure for occupation
            for (int s = 0; s < this->states; ++s) {
                for (int m = 0; m < this->M; ++m) {
                    delete[] occupation[s][m];
                }
                delete[] occupation[s];
            }
            delete[] occupation;

            // Remove the temporary structures for backward and forward
            for (int s = 0; s < this->states; s++) {
                delete[] forward[s];
                delete[] backward[s];
            }
            delete[] forward;
            delete[] backward;
        }

        // Update initial probabilities
        for (int s = 0; s < this->states; ++s) {
            if (utterancesNb == 0) {
                // If none of utterances were starting in this state, then set the initial probability
                // to zero, to avoid having NaN due to division by zero.
                this->pi[s] = 0.0;
                continue;
            }

            this->pi[s] = initialAcc[s] / utterancesNb;
        }

        // Update transition matrix
        for (int s1 = 0; s1 < this->states; ++s1) {
            for (int s2 = 0; s2 < this->states; ++s2) {
                double globalOccupation = 0.0;
                for (int m = 0; m < this->M; ++m) {
                    globalOccupation += occupationsAcc[s1][m];
                }

                if (globalOccupation == 0.0) {
                    // If the global occupation is equal to zero, then set the transition probability to zero,
                    // in order to avoid having NaNs due to division by zero.
                    this->transition[s1][s2] = 0.0;
                    continue;
                }

                this->transition[s1][s2] = transitionsAcc[s1][s2] / globalOccupation;
            }
        }

        this->normalizePi();
        this->normalizeTransitionsMatrix();

        // Update mixtures
        for (int s = 0; s < this->states; ++s) {
            // Calculate occupation of whole GMM
            double gmmOccupation = EPS;
            for (int m = 0; m < this->M; ++m) {
                gmmOccupation += occupationsAcc[s][m];
            }

            GMMLikelihoodFunction &stateGMM = this->hiddenStates->at(s);

            // Update mixture weights
            for (int m = 0; m < this->M; ++m) {
                double mixtureWeight = occupationsAcc[s][m] / gmmOccupation;
                stateGMM.setWeight(m, mixtureWeight);
            }

            // Update mixture means
            for (int m = 0; m < this->M; ++m) {
                stateGMM.setMeans(m, weightedObservationAcc[s][m] / (occupationsAcc[s][m] + EPS));
            }

            // Update mixture variances
            for (int m = 0; m < this->M; ++m) {
                // In order to avoid having variances equal to 0, what may happen when there is only
                // one vector assigned to a particular Gaussian, we use a const minimal variance.
                stateGMM.setVariances(m, weightedVarianceAcc[s][m] / (occupationsAcc[s][m] + EPS));
            }
        }

        // Remove accumulators
        delete[] initialAcc;
        for (int s1 = 0; s1 < this->states; s1++) {
            delete[] transitionsAcc[s1];
            delete[] occupationsAcc[s1];
            delete[] weightedObservationAcc[s1];
            delete[] weightedVarianceAcc[s1];
        }
        delete[] transitionsAcc;
        delete[] occupationsAcc;
        delete[] weightedObservationAcc;
        delete[] weightedVarianceAcc;

        this->displayPi();
        this->displayTransitionsMatrix();
        this->displayHiddenStates();

        std::cout << "Iteration " << iteration << ". logLikelihood = " << logLikelihood << std::endl;
    }
}

double speech::HMMLexicon::MultivariateGaussianHMM::predict(const Observation &observation) {
    unsigned int vectorsNb = observation.size();
    if (vectorsNb == 0) {
        return 0.0;
    }

    unsigned int T = vectorsNb - 1;

    // Create a structure necessary for the forward estimate
    double **forward = new double *[this->states];
    for (int s = 0; s < this->states; s++) {
        forward[s] = new double[vectorsNb]; // alpha_t(i) -> forward[i][t]
    }

    // Calculate the forward estimates for the first time frame
    const valarray<double> &firstVector = observation[0];
    for (int i = 0; i < this->states; ++i) {
        GMMLikelihoodFunction &stateGMM = this->hiddenStates->at(i);
        forward[i][0] = this->pi[i] * stateGMM(firstVector);
    }

    // Calculate the forward estimates for all other time frames
    for (int j = 0; j < this->states; j++) {
        GMMLikelihoodFunction &stateGMM = this->hiddenStates->at(j);
        for (int t = 1; t < vectorsNb; ++t) {
            const valarray<double> &vector = observation[t];

            double sum = 0.0;
            for (int i = 0; i < this->states; i++) {
                sum += forward[i][t - 1] * this->transition[i][j];
            }

            forward[j][t] = sum * stateGMM(vector) + EPS;
        }
    }

    // Accumulate logprob
    double likelihood = EPS;
    for (int s = 0; s < this->states; s++) {
        likelihood += forward[s][T];
    }

    // Remove the temporary structure
    for (int s = 0; s < this->states; s++) {
        delete[] forward[s];
    }
    delete[] forward;

    return likelihood;
}

void speech::HMMLexicon::MultivariateGaussianHMM::initializeMixtures() {
    int observationsNb = 0;
    valarray<double> mean(0.0, this->dimensionality);
    valarray<double> variance(0.0, this->dimensionality);
    for (auto it = this->utterances->begin(); it != this->utterances->end(); ++it) {
        Observation &observation = *it;
        observationsNb += observation.size();
    }

    for (auto it = this->utterances->begin(); it != this->utterances->end(); ++it) {
        Observation &observation = *it;
        for (auto vectorIt = observation.begin(); vectorIt != observation.end(); ++vectorIt) {
            valarray<double> &vector = *vectorIt;
            mean += vector / (double) observationsNb;
        }
    }

    for (auto it = this->utterances->begin(); it != this->utterances->end(); ++it) {
        Observation &observation = *it;
        for (auto vectorIt = observation.begin(); vectorIt != observation.end(); ++vectorIt) {
            valarray<double> &vector = *vectorIt;
            valarray<double> difference = vector - mean;
            valarray<double> product = difference * difference;
            variance += product / (double) observationsNb;
        }
    }

    // Divide each utterance to have N parts, where N is a number of hidden states
    // Select M random vectors to be the initial means of the mixtures
    srand(time(NULL));
    unsigned int nbUtterances = this->utterances->size();
    for (int n = 0; n < this->states; ++n) {
        GMMLikelihoodFunction &stateGMM = this->hiddenStates->at(n);
        for (int m = 0; m < this->M; ++m) {
            int utteranceIdx = random() % nbUtterances;
            Observation &utterance = this->utterances->at(utteranceIdx);
            int nbSamples = utterance.size();
            int rangeWidth = nbSamples / this->states;
            int beginIndex = n * rangeWidth;
            int randomStateIndex = rand() % rangeWidth + beginIndex; // random vector
            stateGMM.setMeans(m, utterance[randomStateIndex]); // set means of the mixture to randomly chosen vector
            stateGMM.setVariances(m, variance);
        }
    }

    this->displayHiddenStates();
}

void speech::HMMLexicon::MultivariateGaussianHMM::initializeModel() {
    // Initialize transitions matrix
    srand(time(NULL));
    for (int s1 = 0; s1 < this->states; s1++) {
        double sum = 0.0;
        for (int s2 = 0; s2 < this->states; s2++) {
            this->transition[s1][s2] = 0.5 + (random() / (double) RAND_MAX);
            sum += this->transition[s1][s2];
        }
        for (int j = 0; j < this->states; j++) {
            this->transition[s1][j] /= sum;
        }
    }

    // Initialize initial probabilities
    for (int s = 0; s < this->states; s++) {
        this->pi[s] = 1.0 / this->states;
    }

    this->displayTransitionsMatrix();
    this->displayPi();
}

void speech::HMMLexicon::MultivariateGaussianHMM::displayHiddenStates() const {
    for (int s = 0; s < this->states; ++s) {
        GMMLikelihoodFunction &stateGMM = this->hiddenStates->at(s);
        for (int m = 0; m < this->M; ++m) {
            std::cout << "state " << s << " / " << m << " info" << std::endl;
            std::cout << "weight: " << stateGMM.getWeight(m) << std::endl;
            std::cout << "means: " << stateGMM.getMeans(m) << std::endl;
            std::cout << "variances: " << stateGMM.getVariances(m) << std::endl;
        }
    }
}

void speech::HMMLexicon::MultivariateGaussianHMM::displayTransitionsMatrix() const {
    std::cout << "Transitions: " << std::endl;
    for (int s1 = 0; s1 < this->states; s1++) {
        for (int s2 = 0; s2 < this->states; s2++) {
            std::cout << this->transition[s1][s2] << '\t';
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void speech::HMMLexicon::MultivariateGaussianHMM::displayPi() const {
    std::cout << "Initials: " << std::endl;
    for (int s = 0; s < this->states; s++) {
        std::cout << this->pi[s] << ' ';
    }
    std::cout << std::endl;
}

void speech::HMMLexicon::MultivariateGaussianHMM::normalizeTransitionsMatrix() {
    for (int s1 = 0; s1 < this->states; ++s1) {
        double sum = this->states * EPS; // to ensure it is not zero
        for (int s2 = 0; s2 < this->states; ++s2) {
            sum += this->transition[s1][s2];
        }

        if (sum == 0.0) {
            // There were no transitions from this state, so let assume that we should
            // go wherever we want to, with the same probability
            for (int s2 = 0; s2 < this->states; ++s2) {
                this->transition[s1][s2] = 0.0;
            }

            this->transition[s1][s1] = 1.0;
            continue;
        }

        for (int s2 = 0; s2 < this->states; ++s2) {
            this->transition[s1][s2] += EPS;
            this->transition[s1][s2] /= sum;
        }
    }
}

void speech::HMMLexicon::MultivariateGaussianHMM::normalizePi() {
    double sum = EPS;
    for (int s = 0; s < this->states; ++s) {
        sum += this->pi[s];
    }

    for (int s = 0; s < this->states; ++s) {
        this->pi[s] /= sum;
    }
}