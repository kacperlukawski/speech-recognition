//
// Created by kacper on 04.06.15.
//

#include "HMMLexicon.h"
#include "helpers.h"
#include "operators.h"

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
    if (unitModels.find(transcription) == unitModels.end()) {
        // given transcription was not passed before
        vector<string> substates = this->split(transcription, unitSeparator);
        int states = substates.size() + 1; // one additional state is for the end state
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

static double EPS = 10e-10; // TODO: move into the class

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

    // Create temporary structures used to accumulate data between iterations
    double *initialAcc = new double[this->states];
    double **transitionsAcc = new double *[this->states];
    double **occupationsAcc = new double *[this->states];
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
            forward[i][0] = this->pi[i] * stateGMM(firstVector);
            std::cout << "forward[" << i << "][" << 0 << "] = " << forward[i][0] << std::endl;
        }

        // Calculate the forward estimates for all other time frames
        for (int j = 0; j < this->states; j++) {
            GMMLikelihoodFunction &stateGMM = this->hiddenStates->at(j);
            for (int t = 1; t < vectorsNb; ++t) {
                valarray<double> &vector = observation[t];

                double sum = 0.0;
                for (int i = 0; i < this->states; i++) {
                    sum += forward[i][t - 1] * this->transition[i][j];
                }

                forward[j][t] = sum * stateGMM(vector);
                std::cout << "forward[" << j << "][" << t << "] = " << forward[j][t] << std::endl;
            }
        }

        // Calculate the backward estimates for the last time frame
        int T = vectorsNb - 1;
        for (int i = 0; i < this->states; i++) {
            backward[i][T] = 1.0;
        }

        // Calculate the backward estimates for all other time frames
        for (int i = 0; i < this->states; ++i) {
            for (int t = T - 1; t >= 0; --t) {
                valarray<double> &nextVector = observation[t + 1];
                backward[i][t] = 0.0;
                for (int j = 0; j < this->states; ++j) {
                    GMMLikelihoodFunction &stateGMM = this->hiddenStates->at(j);
                    backward[i][t] += this->transition[i][j] * stateGMM(nextVector) * backward[j][t + 1];
                }

                std::cout << "backward[" << i << "][" << t << "] = " << backward[i][t] << std::endl;
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

        std::cout << std::endl;

        // Fill the state occupation estimates
        for (int j = 0; j < this->states; ++j) {
            GMMLikelihoodFunction &stateGMM = this->hiddenStates->at(j);
            for (int t = 0; t < vectorsNb; ++t) {
                double denominator = 0.0;
                for (int i = 0; i < this->states; ++i) {
                    denominator += forward[i][t] * backward[i][t];
                }

                valarray<double> &vector = observation[t];
                double vectorProbability = stateGMM(vector);
                double globalOccupation = (forward[j][t] / denominator) * backward[j][t]; // / denominator;
                double alternative = exp(log(forward[j][t]) + log(backward[j][t]) - log(denominator));

                std::cout << "forward[" << j << "][" << t << "] = " << forward[j][t] << std::endl;
                std::cout << "backward[" << j << "][" << t << "] = " << backward[j][t] << std::endl;
                std::cout << "denominator = " << denominator << std::endl;
                std::cout << "globalOccupation = " << globalOccupation << std::endl;
                std::cout << "alternative = " << alternative << std::endl;


                for (int m = 0; m < this->M; ++m) {
                    double vectorProbabilityInMixture = stateGMM(m, vector);
                    occupation[j][m][t] = globalOccupation * (vectorProbabilityInMixture / vectorProbability);
//                    std::cout << "vectorProbabilityInMixture = " << vectorProbabilityInMixture << std::endl;
//                    std::cout << "vectorProbability = " << vectorProbability << std::endl;

                    std::cout << "occupation[" << j << "][" << m << "][" << t << "] = " << occupation[j][m][t] <<
                                                                                           std::endl << std::endl;
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
                            alternativeDenominator += forward[k][t] * this->transition[k][l] * nextStateProbabilityOfVector * backward[l][t + 1];
                        }
                    }

                    double transition = globalOccupation * nextVectorProbability * this->transition[i][j] * backward[j][t + 1] / backward[i][t];
                    double alternativeTransition = forward[i][t] * this->transition[i][j] * nextVectorProbability * backward[j][t + 1] / alternativeDenominator;

                    double test = exp(log(globalOccupation) + log(nextVectorProbability) + log(this->transition[i][j]) + log(backward[j][t + 1]) - log(backward[i][t]));
                    transitions[i][j][t] = alternativeTransition; // was transition before
//                    std::cout << "globalOccupation = " << globalOccupation << std::endl;
//                    std::cout << "nextVectorProbability = " << nextVectorProbability << std::endl;
//                    std::cout << "this->transition[" << i << "][" << j << "] = " << this->transition[i][j] << std::endl;
//                    std::cout << "backward[" << j << "][" << (t + 1) << "] = " << backward[j][t + 1] << std::endl;
//                    std::cout << "backward[" << i << "][" << t << "] = " << backward[i][t] << std::endl;
                    std::cout << "transitions[" << i << "][" << j << "][" << t << "] = " << transitions[i][j][t] << std::endl;
                    std::cout << "alternativeTransition = " << alternativeTransition << std::endl;
                    std::cout << "test = " << test << std::endl << std::endl; // TODO: check why transitions value is usually higher than one
                }
            }
        }

        // Accumulate initial probabilities
        // IS WORKING FINE!
        for (int s = 0; s < this->states; ++s) {
            double sum = 0.0;
            for (int m = 0; m < this->M; ++m) {
                sum += occupation[s][m][0];
            }

            initialAcc[s] += sum;
        }

        // Accumulate transitions
        // TODO: check why calculated transitions are higher than 1
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
            for (int m = 0; m < this->M; ++m) {
                double sum = 0.0;
                for (int t = 0; t < vectorsNb; ++t) {
                    sum += occupation[s][m][t];
                }

                occupationsAcc[s][m] += sum;
            }
        }

        // TODO: Accumulate all values

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

    // TOOD: check why using these values lead to really low or really high values insted of returning something summing up to 1

    // Update initial probabilities
    for (int s = 0; s < this->states; ++s) {
        this->pi[s] = initialAcc[s] / utterancesNb;
    }

    // Update transition matrix
    for (int s1 = 0; s1 < this->states; ++s1) {
        for (int s2 = 0; s2 < this->states; ++s2) {
            double globalOccupation = 0.0;
            for (int m = 0; m < this->M; ++m) {
                globalOccupation += occupationsAcc[s1][m];
            }

            std::cout << "transitionsAcc[" << s1 << "][" << s2 << "] = " << transitionsAcc[s1][s2] << std::endl;
            std::cout << "globalOccupation = " << globalOccupation << std::endl;
            this->transition[s1][s2] = transitionsAcc[s1][s2] / globalOccupation;
        }
    }

    // Remove accumulators
    delete[] initialAcc;
    for (int s1 = 0; s1 < this->states; s1++) {
        delete[] transitionsAcc[s1];
        delete[] occupationsAcc[s1];
    }
    delete[] transitionsAcc;
    delete[] occupationsAcc;

    this->displayPi();
    this->displayTransitionsMatrix();
}

void speech::HMMLexicon::MultivariateGaussianHMM::initializeMixtures() {
    // Get the minimal and maximal values of each dimension of the input vectors.
    valarray<double> min(0.0, this->dimensionality);
    valarray<double> max(0.0, this->dimensionality);
    for (auto it = this->utterances->begin(); it != this->utterances->end(); ++it) {
        Observation &observation = *it;
        for (auto vectorIt = observation.begin(); vectorIt != observation.end(); ++vectorIt) {
            valarray<double> &vector = *vectorIt;
            min = minItems(min, vector);
            max = maxItems(max, vector);
        }
    }

    std::cout << "min vector: " << min << std::endl;
    std::cout << "max vector: " << max << std::endl;

    // For each hidden state representing language unit, generate each mixture mean randomly, but from the range
    // defined by min and max value of each dimension. Variance is defined as range / M, where M is number of mixtures
    // and range is a difference between min and max value. Weight is equal for all mixtures (1 / M).
    srand(time(NULL));
    for (int s = 0; s < this->states; ++s) {
        GMMLikelihoodFunction &stateGMM = this->hiddenStates->at(s);
        for (int m = 0; m < this->M; ++m) {
            stateGMM.setWeight(m, 1.0 / this->M);
            for (int d = 0; d < this->dimensionality; ++d) {
                double range = max[d] - min[d];
                double variance = range / this->M;
                double mean = (rand() / (double) RAND_MAX) * range + min[d];
                stateGMM.setMean(m, d, mean);
                stateGMM.setVariance(m, d, variance);
            }

            std::cout << "state " << s << " / " << m << " info" << std::endl;
            std::cout << "weight: " << stateGMM.getWeight(m) << std::endl;
            std::cout << "means: " << stateGMM.getMeans(m) << std::endl;
            std::cout << "variances: " << stateGMM.getVariances(m) << std::endl;
        }
    }
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