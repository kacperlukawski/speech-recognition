//
// Created by kacper on 04.06.15.
//

#include "HMMLexicon.h"

speech::HMMLexicon::HMMLexicon(int dimensionality, unsigned int gaussians,
                               std::shared_ptr<speech::initializer::AbstractGaussianInitializer> initializer)
        : dimensionality(dimensionality), gaussians(gaussians), initializer(initializer) {
}

speech::HMMLexicon::~HMMLexicon() {
    for (auto it = unitModels.begin(); it != unitModels.end(); ++it) {
        delete it->second;
    }
}

void speech::HMMLexicon::addUtterance(Observation utterance,
                                      string transcription,
                                      string unitSeparator) {
    if (unitModels.find(transcription) == unitModels.end()) {
        // given transcription was not passed before
        vector<string> substates = this->split(transcription, unitSeparator);
        int states = substates.size() + 1; // one additional state is for the end state
        unitModels[transcription] = new MultivariateGaussianHMM(this->dimensionality, states, gaussians,
                                                                this->initializer);
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

        if (it->second > maxLikelihood && !isinf(it->second)) {
            maxLikelihood = it->second;
            bestLabel = it->first;
        }
    }

    return bestLabel;
}

void speech::HMMLexicon::fit() {
#pragma omp parallel
#pragma omp single nowait
    {
        for (auto it = this->unitModels.begin(); it != this->unitModels.end(); ++it) {
#pragma omp task firstprivate(it)
            {
                std::cout << "Fitting model: " << it->first << std::endl;
                it->second->fit();
            }
        }
    }
}

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
        result += (*this)(i, observation);
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
    double varianceNorm = this->variances[k].max(); // Euclidean norm of matrix is a highest eigenvalue, but eigenvalues of the diagonal matrix are their non-zero values
    valarray<double> invVariance = valarray<double>(1.0, D) / this->variances[k];
    double power = (difference * invVariance * difference).sum();
    return this->weights[k] * exp(-0.5 * power) / sqrt(pow(2 * M_PI, this->D) * varianceNorm);
}

constexpr double speech::HMMLexicon::MultivariateGaussianHMM::EPS;
constexpr double speech::HMMLexicon::MultivariateGaussianHMM::MIN_VARIANCE;

speech::HMMLexicon::MultivariateGaussianHMM::MultivariateGaussianHMM(unsigned int dimensionality, unsigned int states,
                                                                     unsigned int M,
                                                                     std::shared_ptr<speech::initializer::AbstractGaussianInitializer> initializer)
        : dimensionality(dimensionality), states(states), M(M), initializer(initializer) {
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
        double **transitionsAcc = createArray<double>(this->states, this->states);
        double **occupationsAcc = createArray<double>(this->states, this->M); // [state][mixture]
        valarray<double> **weightedObservationAcc = createArray<valarray<double>>(this->states, this->M);
        valarray<double> **weightedVarianceAcc = createArray<valarray<double>>(this->states, this->M);
        for (int s1 = 0; s1 < this->states; ++s1) {
            initialAcc[s1] = 0.0;

            for (int s2 = 0; s2 < this->states; ++s2) {
                transitionsAcc[s1][s2] = 0.0;
            }

            for (int m = 0; m < this->M; ++m) {
                occupationsAcc[s1][m] = 0.0;
                weightedObservationAcc[s1][m] = valarray<double>(0.0, this->dimensionality);
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

            unsigned int T = vectorsNb - 1; // max time frame

            // Create a structures necessary for the forward and backward estimates
            double **forward = createArray<double>(this->states, vectorsNb);
            double **backward = createArray<double>(this->states, vectorsNb);

            // Calculate the forward and backward probabilities
            this->calculateForwardProbabilities(forward, observation, true);
            this->calculateBackwardProbabilities(backward, observation, true);

            // Create a structure necessary for occupation estimates
            double ***occupation = createArray<double>(this->states, this->M, vectorsNb);

            // Calculate the estimations of occupation probabilities
            this->calculateOccupationEstimates(occupation, forward, backward, observation);

            // Create a structure necessary for transition estimates
            double ***transitions = createArray<double>(this->states, this->states, vectorsNb);

            // Calculate the estimations of transition probabilities
            this->calculateTransitionEstimates(transitions, forward, backward, occupation, observation);

            // Accumulate initial probabilities
            this->accumulateInitialProbabilities(initialAcc, occupation);

            // Accumulate transitions
            this->accumulateTransitions(transitionsAcc, transitions, observation);

            // Accumulate mixtures occupations
            this->accumulateMixtureOccupations(occupationsAcc, occupation, observation);

            // Accumulate new mixtures means and variances
            this->accumulateMixtureMeans(weightedObservationAcc, occupation, occupationsAcc, observation);
            this->accumulateMixtureVariances(weightedVarianceAcc, weightedObservationAcc, occupation, occupationsAcc,
                                             observation);

            // Display forward
#ifdef SPEECH_VERBOSITY
            std::cout << "Forward / Backward:" << std::endl;
            for (int t = 0; t <= T; t++) {
                for (int s = 0; s < this->states; s++) {
                    std::cout << forward[s][t] << ' ';
                }
                std::cout << "\t\t";
                for (int s = 0; s < this->states; s++) {
                    std::cout << backward[s][t] << ' ';
                }
                std::cout << std::endl;
            }
            std::cout << std::endl;
#endif
            // Calculate log-likelihood of the current observation
            logLikelihood += calculateLogLikelihood(observation);

            // Remove the temporary structure for transition estimates
            removeArray(transitions, this->states, this->states);

            // Remove the temporary structure for occupation
            removeArray(occupation, this->states, this->M);

            // Remove the temporary structures for backward and forward
            removeArray(forward, this->states);
            removeArray(backward, this->states);
        }

        // Update initial probabilities
        for (int s = 0; s < this->states; ++s) {
            this->pi[s] = initialAcc[s] / utterancesNb;
        }

        // Update transition matrix
        for (int s1 = 0; s1 < this->states; ++s1) {
            double stateOccupation = 0.0;
            for (int m = 0; m < this->M; ++m) {
                stateOccupation += occupationsAcc[s1][m];
            }

            for (int s2 = 0; s2 < this->states; ++s2) {
                this->transition[s1][s2] = transitionsAcc[s1][s2] / (stateOccupation + EPS);
            }
        }

//        this->normalizePi();
        this->normalizeTransitionsMatrix();

        // Update the weights, means and variances of Gaussian mixtures
        this->updateMixtures(occupationsAcc, weightedObservationAcc, weightedVarianceAcc);

        // Remove accumulators
        removeArray(transitionsAcc, this->states);
        removeArray(occupationsAcc, this->states);
        removeArray(weightedObservationAcc, this->states);
        removeArray(weightedVarianceAcc, this->states);
        delete[] initialAcc;

        // Displays current configuration of the model
        this->displayHiddenStates();
        this->displayPi();
        this->displayTransitionsMatrix();

        std::cout << "Iteration " << iteration << ". logLikelihood = " << logLikelihood << std::endl;
    }
}

double speech::HMMLexicon::MultivariateGaussianHMM::predict(const Observation &observation) {
    return this->calculateLogLikelihood(observation);
}

void speech::HMMLexicon::MultivariateGaussianHMM::initializeMixtures() {
//    int observationsNb = 0;
//    valarray<double> mean(0.0, this->dimensionality);
//    valarray<double> variance(0.0, this->dimensionality);
//    for (auto it = this->utterances->begin(); it != this->utterances->end(); ++it) {
//        Observation &observation = *it;
//        observationsNb += observation.size();
//    }
//
//    // Calculates the mean of all observations
//    for (auto it = this->utterances->begin(); it != this->utterances->end(); ++it) {
//        Observation &observation = *it;
//        for (auto vectorIt = observation.begin(); vectorIt != observation.end(); ++vectorIt) {
//            valarray<double> &vector = *vectorIt;
//            mean += vector / (double) observationsNb;
//        }
//    }
//
//    // Calculates global variance of the data
//    for (auto it = this->utterances->begin(); it != this->utterances->end(); ++it) {
//        Observation &observation = *it;
//        for (auto vectorIt = observation.begin(); vectorIt != observation.end(); ++vectorIt) {
//            valarray<double> &vector = *vectorIt;
//            valarray<double> difference = vector - mean;
//            valarray<double> product = difference * difference;
//            variance += product / (double) observationsNb;
//        }
//    }
//
//    // Divide each utterance to have N parts, where N is a number of hidden states
//    // Select M random vectors to be the initial means of the mixtures
//    srand(time(NULL));
//    unsigned int nbUtterances = this->utterances->size();
//    for (int n = 0; n < this->states; ++n) {
//        GMMLikelihoodFunction &stateGMM = this->hiddenStates->at(n);
//        for (int m = 0; m < this->M; ++m) {
//            int utteranceIdx = random() % nbUtterances;
//            Observation &utterance = this->utterances->at(utteranceIdx);
//            int nbSamples = utterance.size();
//            int rangeWidth = nbSamples / this->states;
//            int beginIndex = n * rangeWidth;
//            int randomStateIndex = rand() % rangeWidth + beginIndex; // random vector
//            stateGMM.setMeans(m, utterance[randomStateIndex]); // set means of the mixture to randomly chosen vector
//            stateGMM.setVariances(m, variance);
//        }
//    }

    this->initializer->initialize(*this);
    this->displayHiddenStates();
}

void speech::HMMLexicon::MultivariateGaussianHMM::initializeModel() {
    // Initialize transitions matrix
//    srand(time(NULL));
//    for (int s1 = 0; s1 < this->states; s1++) {
//        double sum = 0.0;
//        for (int s2 = 0; s2 < this->states; s2++) {
//            this->transition[s1][s2] = 0.5 + (random() / (double) RAND_MAX);
//            sum += this->transition[s1][s2];
//        }
//        for (int j = 0; j < this->states; j++) {
//            this->transition[s1][j] /= sum;
//        }
//    }

    for (int s1 = 0; s1 < this->states; s1++) {
        for (int s2 = 0; s2 < this->states; s2++) {
            this->transition[s1][s2] = 0.0;
        }

        if (s1 + 1 < this->states) {
            this->transition[s1][s1] = 0.5;
            this->transition[s1][s1 + 1] = 0.5;
        } else {
            this->transition[s1][s1] = 1.0;
        }
    }

    // Initialize initial probabilities
//    for (int s = 0; s < this->states; s++) {
//        this->pi[s] = 1.0 / this->states;
//    }

    for (int s = 0; s < this->states; s++) {
        this->pi[s] = 0.0;
    }
    this->pi[0] = 1.0;

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
//            this->transition[s1][s2] += EPS;
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

double speech::HMMLexicon::MultivariateGaussianHMM::calculateLogLikelihood(const Observation &observation) {
    // TODO: find a way to calculate log-likelihood more precisely
    unsigned int vectorsNb = observation.size();
    unsigned int T = vectorsNb - 1;
    if (T <= 0) {
        return 0.0;
    }

    double **logForward = createArray<double>(this->states, vectorsNb);

    const valarray<double> &firstVector = observation.at(0);
    for (int i = 0; i < this->states; i++) {
        GMMLikelihoodFunction &stateGMM = this->hiddenStates->at(i);
        double stateOccupation = stateGMM(firstVector);
        logForward[i][0] = log(this->pi[i] + EPS) + log(stateOccupation + EPS);
    }


    for (int t = 1; t <= T; t++) {
        const valarray<double> &vector = observation.at(t);

        for (int j = 0; j < this->states; j++) {
            GMMLikelihoodFunction &stateGMM = this->hiddenStates->at(j);
            double stateOccupation = stateGMM(vector);

            double previousStatesForwardSum = 0.0;
            for (int i = 0; i < this->states; i++) {
                previousStatesForwardSum += exp(logForward[i][t - 1] + log(this->transition[i][j] + EPS));
            }

            logForward[j][t] = log(stateOccupation + EPS) + log(previousStatesForwardSum + EPS);
        }
    }

    double likelihood = 0.0;
    for (int i = 0; i < this->states; i++) {
        likelihood += exp(
                logForward[i][T]); // TODO: it is a sum of log-probabilities - product of normal probabilities CORRECT
    }

    removeArray<double>(logForward, this->states);

    return log(likelihood);
}

void speech::HMMLexicon::MultivariateGaussianHMM::calculateForwardProbabilities(double **forward,
                                                                                const Observation &observation,
                                                                                bool normalize) {
    unsigned int vectorsNb = observation.size();
    unsigned int T = vectorsNb - 1;

    const valarray<double> &firstVector = observation.at(0);

    double probabilitiesSum = 0.0;
    for (int i = 0; i < this->states; ++i) {
        GMMLikelihoodFunction &stateGMM = this->hiddenStates->at(i);
        double stateOccupation = stateGMM(firstVector);
        forward[i][0] = this->pi[i] * stateOccupation;
        probabilitiesSum += forward[i][0];
    }

    if (normalize) {
        for (int i = 0; i < this->states; ++i) {
            forward[i][0] /= probabilitiesSum + EPS;
        }
    }

    // Calculate the forward estimates for all other time frames
    for (int t = 1; t <= T; t++) {
        const valarray<double> &vector = observation.at(t);

        double probabilitiesSum = 0.0;
        for (int j = 0; j < this->states; j++) {
            GMMLikelihoodFunction &stateGMM = this->hiddenStates->at(j);
            double stateOccupation = stateGMM(vector); // stateOccupation is sometimes 0

            double sum = 0.0;
            for (int i = 0; i < this->states; i++) {
                sum += forward[i][t - 1] * this->transition[i][j];
            }

            forward[j][t] = sum * stateOccupation + EPS;
            probabilitiesSum += forward[j][t];
        }

        if (normalize) {
            for (int j = 0; j < this->states; j++) {
                forward[j][t] /= probabilitiesSum + EPS;
            }
        }
    }
}

void speech::HMMLexicon::MultivariateGaussianHMM::calculateBackwardProbabilities(double **backward,
                                                                                 const Observation &observation,
                                                                                 bool normalize) {
    unsigned int vectorsNb = observation.size();
    unsigned int T = vectorsNb - 1;

    // Calculate the backward estimates for the last time frame
    double probabilitiesSum = 0.0;
    for (int i = 0; i < this->states; i++) {
        GMMLikelihoodFunction &stateGMM = this->hiddenStates->at(i);
        backward[i][T] = 1.0; //stateGMM(observation[T]);// 1.0;
        probabilitiesSum += backward[i][T];
    }

    if (normalize) {
        for (int i = 0; i < this->states; i++) {
            backward[i][T] /= probabilitiesSum + EPS;
        }
    }

    // Calculate the backward estimates for all other time frames
    for (int t = T - 1; t >= 0; --t) {
        const valarray<double> &nextVector = observation.at(t + 1);

        double probabilitiesSum = 0.0;
        for (int i = 0; i < this->states; ++i) {
            backward[i][t] = EPS;
            for (int j = 0; j < this->states; ++j) {
                GMMLikelihoodFunction &stateGMM = this->hiddenStates->at(j);
                double stateOccupation = stateGMM(nextVector);
                backward[i][t] += this->transition[i][j] * stateOccupation * backward[j][t + 1];
            }

            probabilitiesSum += backward[i][t];
        }

        if (normalize) {
            for (int i = 0; i < this->states; i++) {
                backward[i][t] /= probabilitiesSum + EPS;
            }
        }
    }
}

void speech::HMMLexicon::MultivariateGaussianHMM::calculateOccupationEstimates(double ***occupation, double **forward,
                                                                               double **backward,
                                                                               const Observation &observation) {
    unsigned int T = observation.size() - 1;
    // Fill the state occupation estimates
    for (int t = 0; t <= T; ++t) {
        const valarray<double> &vector = observation.at(t);

        double overallProbability = 0.0;
        for (int i = 0; i < this->states; ++i) {
            overallProbability += forward[i][t] * backward[i][t];
        }

        for (int j = 0; j < this->states; ++j) {
            GMMLikelihoodFunction &stateGMM = this->hiddenStates->at(j);
            double vectorProbability = stateGMM(vector);
            double overallStateOccupation = forward[j][t] * backward[j][t] / (overallProbability + EPS);

            for (int m = 0; m < this->M; ++m) {
                double vectorProbabilityInMixture = stateGMM(m, vector);
                double mixtureWeight = vectorProbabilityInMixture / (vectorProbability + EPS);
                occupation[j][m][t] = overallStateOccupation * mixtureWeight;
            }
        }
    }
}

void speech::HMMLexicon::MultivariateGaussianHMM::calculateTransitionEstimates(double ***transitions, double **forward,
                                                                               double **backward,
                                                                               double ***occupation,
                                                                               const Observation &observation) {
    unsigned int T = observation.size() - 1;
    // Calculate the transition estimates
    for (int t = 0; t < T; ++t) {
        const valarray<double> &nextVector = observation.at(t + 1);

        double overallTransition = 0.0;
        for (int k = 0; k < this->states; k++) {
            for (int l = 0; l < this->states; l++) {
                GMMLikelihoodFunction &nextStateGMM = this->hiddenStates->at(l);
                overallTransition +=
                        forward[k][t] * this->transition[k][l] * nextStateGMM(nextVector) * backward[l][t + 1];
            }
        }

        for (int s1 = 0; s1 < this->states; ++s1) {
            for (int s2 = 0; s2 < this->states; ++s2) {
                // TODO: check if the transition estimate is correct, because inner state transition strives to zero
                GMMLikelihoodFunction &stateGMM = this->hiddenStates->at(s2);
                double globalOccupation = 0.0;
                for (int m = 0; m < this->M; ++m) {
                    globalOccupation += occupation[s1][m][t];
                }

                double nextVectorProbability = stateGMM(nextVector);
                double stateToStateTransition =
                        forward[s1][t] * this->transition[s1][s2] * nextVectorProbability * backward[s2][t + 1];
                transitions[s1][s2][t] = stateToStateTransition / (overallTransition + EPS);
            }
        }
    }
}

void speech::HMMLexicon::MultivariateGaussianHMM::accumulateInitialProbabilities(double *initialAcc,
                                                                                 double ***occupation) {
    for (int s = 0; s < this->states; ++s) {
        double sum = 0.0;
        for (int m = 0; m < this->M; ++m) {
            sum += occupation[s][m][0];
        }

        initialAcc[s] += sum;
    }
}

void speech::HMMLexicon::MultivariateGaussianHMM::accumulateTransitions(double **transitionsAcc,
                                                                        double ***transitions,
                                                                        const Observation &observation) {
    unsigned int vectorsNb = observation.size();
    unsigned int T = vectorsNb - 1;
    for (int s1 = 0; s1 < this->states; ++s1) {
        for (int s2 = 0; s2 < this->states; ++s2) {
            double transitionsOverTime = 0.0;
            for (int t = 0; t < T; ++t) {
                transitionsOverTime += transitions[s1][s2][t];
            }

            transitionsAcc[s1][s2] += transitionsOverTime;
        }
    }
}


void speech::HMMLexicon::MultivariateGaussianHMM::accumulateMixtureOccupations(double **occupationsAcc,
                                                                               double ***occupation,
                                                                               const Observation &observation) {
    unsigned int vectorsNb = observation.size();
    unsigned int T = vectorsNb - 1;
    for (int s = 0; s < this->states; ++s) {
        GMMLikelihoodFunction &stateGMM = this->hiddenStates->at(s);
        for (int m = 0; m < this->M; ++m) {
            // Calculates global occupation of this particular mixture
            double mixtureOccupation = 0.0;
            for (int t = 0; t < vectorsNb; ++t) {
                if (isnan(occupation[s][m][t])) {
                    occupation[s][m][t] = 0.0;
                }

                mixtureOccupation += occupation[s][m][t];
            }

            occupationsAcc[s][m] += mixtureOccupation;

//            const valarray<double> &mixtureMean = stateGMM.getMeans(m);
//            for (int t = 0; t < vectorsNb; ++t) {
//                double observationWeightInMean = occupation[s][m][t];
//                weightedObservationAcc[s][m] += observationWeightInMean * observation[t];
//
//                valarray<double> difference = observation[t] - mixtureMean;
//                valarray<double> product = difference * difference;
//                weightedVarianceAcc[s][m] += observationWeightInMean * product;
//
////                std::cerr << observationWeightInMean << " * " << observation[t] << std::endl;
//            }
        }
    }
}

void speech::HMMLexicon::MultivariateGaussianHMM::accumulateMixtureMeans(valarray<double> **weightedObservationAcc,
                                                                         double ***occupation, double **occupationsAcc,
                                                                         const Observation &observation) {
    unsigned int vectorsNb = observation.size();
    unsigned int T = vectorsNb - 1;
    for (int t = 0; t < vectorsNb; ++t) {
        for (int s = 0; s < this->states; ++s) {
            GMMLikelihoodFunction &stateGMM = this->hiddenStates->at(s);
            for (int m = 0; m < this->M; ++m) {
                double observationWeightInMean = occupation[s][m][t];
                weightedObservationAcc[s][m] += observationWeightInMean * observation[t];
            }
        }
    }
}

void speech::HMMLexicon::MultivariateGaussianHMM::accumulateMixtureVariances(valarray<double> **weightedVarianceAcc,
                                                                             valarray<double> **weightedObservationAcc,
                                                                             double ***occupation,
                                                                             double **occupationsAcc,
                                                                             const Observation &observation) {
    unsigned int vectorsNb = observation.size();
    unsigned int T = vectorsNb - 1;
    for (int s = 0; s < this->states; ++s) {
        GMMLikelihoodFunction &stateGMM = this->hiddenStates->at(s);
        for (int m = 0; m < this->M; ++m) {
            const valarray<double> &mixtureMean = stateGMM.getMeans(m);
//            const valarray<double> newMixtureMean = weightedObservationAcc[s][m] / (occupationsAcc[s][m] + EPS);
//            std::cout << (mixtureMean - newMixtureMean) << std::endl;
            for (int t = 0; t < vectorsNb; ++t) {
                double observationWeightInMean = occupation[s][m][t];
//                weightedObservationAcc[s][m] += observationWeightInMean * observation[t];

                valarray<double> difference = observation[t] - mixtureMean;
                valarray<double> product = difference * difference;
                weightedVarianceAcc[s][m] += observationWeightInMean * product;

//                std::cerr << observationWeightInMean << " * " << observation[t] << std::endl;
            }
        }
    }
}

void speech::HMMLexicon::MultivariateGaussianHMM::updateMixtures(double **occupationsAcc,
                                                                 valarray<double> **weightedObservationAcc,
                                                                 valarray<double> **weightedVarianceAcc) {
    // TODO: normalize the mixture weights
    // Update mixtures
    for (int s = 0; s < this->states; ++s) {
        // Calculate occupation of whole GMM
        double gmmOccupation = EPS * this->M;
        for (int m = 0; m < this->M; ++m) {
            gmmOccupation += occupationsAcc[s][m];
        }

        speech::HMMLexicon::GMMLikelihoodFunction &stateGMM = this->hiddenStates->at(s);

        double weightsSum = 0.0;
        // Update mixture weights
        for (int m = 0; m < this->M; ++m) {
            double mixtureWeight = (occupationsAcc[s][m] + EPS) / gmmOccupation;
            stateGMM.setWeight(m, mixtureWeight);
            weightsSum += mixtureWeight;
        }

        if (weightsSum < 0.99) {
            std::cout << "weightsSum = " << weightsSum << std::endl;
            throw weightsSum;
        }

        // Update mixture means
        for (int m = 0; m < this->M; ++m) {
            stateGMM.setMeans(m, weightedObservationAcc[s][m] /
                                 (occupationsAcc[s][m] + speech::HMMLexicon::MultivariateGaussianHMM::EPS));
        }

        // Update mixture variances
        for (int m = 0; m < this->M; ++m) {
            // In order to avoid having variances equal to 0, what may happen when there is only
            // one vector assigned to a particular Gaussian, we use a const minimal variance.
            stateGMM.setVariances(m, weightedVarianceAcc[s][m] /
                                     (occupationsAcc[s][m] + speech::HMMLexicon::MultivariateGaussianHMM::EPS));
        }
    }
}