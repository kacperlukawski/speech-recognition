//
// Created by kacper on 04.06.15.
//

#include "HMMLexicon.h"
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
        vector <string> substates = this->split(transcription, unitSeparator);
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
        : M(M), D(D), weights(valarray<double>(1.0 / M, M)), means(vector < valarray < double >> (M)),
          variances(vector < valarray < double >> (M)) {
    // initialize means and variances
    for (int i = 0; i < this->M; i++) {
        means[i] = valarray<double>(0.0, D);
        variances[i] = valarray<double>(1.0, D);
    }
}

double speech::HMMLexicon::GMMLikelihoodFunction::operator()(const valarray <double> &observation) {
    if (observation.size() != this->D) {
        throw std::out_of_range("Given observation has wrong dimensionality");
    }

    double result = 0.0;
    for (int i = 0; i < this->M; i++) {
        valarray <double> difference = observation - this->means[i];
        valarray <double> invVariance = valarray<double>(1.0, D) / this->variances[i];
        double varianceNorm = invVariance.max(); // Euclidean norm of matrix is a highest eigenvalue, but eigenvalues of the diagonal matrix are their non-zero values
        double power = ((difference / this->variances[i]) * difference).sum();
        result += this->weights[i] * exp(-0.5 * power) / (pow(2 * M_PI, this->D / 2.0) * varianceNorm);
    }

    return result;
}

double speech::HMMLexicon::GMMLikelihoodFunction::operator()(unsigned int k, const valarray <double> &observation) {
    if (k >= this->M) {
        throw std::out_of_range("Trying to use non-existing k-th mixture");
    }

    if (observation.size() != this->D) {
        throw std::out_of_range("Given observation has wrong dimensionality");
    }

    valarray <double> difference = observation - this->means[k];
    valarray <double> invVariance = valarray<double>(1.0, D) / this->variances[k];
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
        this->hiddenStates->push_back(
                GMMLikelihoodFunction(M, dimensionality)); // assign a multivariate Gaussian for each state
        this->pi[i] = 1.0 / states; // assign start probability of each state
        this->transition[i] = new double[states];
        initTransitionsRandomly(this->transition[i], states);
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

/*void speech::HMMLexicon::MultivariateGaussianHMM::fit() {
    // TODO: set means and variances of the Gaussians using some data coming from the utterances
    // TODO: can use the distance function or KMeans to get an initial point
    // TODO: it might be also good to get the min, max for each dimension and randomly choose centers - algorithm should also converge

    srand(time(NULL));
    for (int s = 0; s < this->states; s++) {
        for (int m = 0; m < this->M; m++) {
            for (int d = 0; d < this->dimensionality; d++) {
                GMMLikelihoodFunction &stateGMM = this->hiddenStates->at(s);
                stateGMM.setMean(m, d, (5.0 * rand() / RAND_MAX) - 2.5); // TODO: use data!
                stateGMM.setVariance(m, d,
                                     3.0);//5.0 + (5.0 * rand() / //RAND_MAX)); // TODO: remove this fix, variance should be much lower
            }
        }
    }

    displayTransitionsMatrix();

    for (int iter = 0; iter < 10; iter++) {
        valarray<double> **recalculatedMeans = new valarray<double> *[states];
        valarray<double> **recalculatedVariances = new valarray<double> *[states];
        double **recalculatedTransitionsNumerators = new double *[this->states];
        double **recalculatedTransitionsDenominators = new double *[this->states];
        double **utterancesSums = new double *[states];
        for (int i = 0; i < states; i++) {
            recalculatedMeans[i] = new valarray<double>[this->M];
            recalculatedVariances[i] = new valarray<double>[this->M];
            recalculatedTransitionsNumerators[i] = new double[this->states];
            recalculatedTransitionsDenominators[i] = new double[this->states];
            utterancesSums[i] = new double[this->M];
            for (int j = 0; j < this->M; j++) {
                recalculatedMeans[i][j] = valarray<double>(0.0, this->dimensionality);
                recalculatedVariances[i][j] = valarray<double>(0.0, this->dimensionality);
                recalculatedTransitionsNumerators[i][j] = 0.0;
                recalculatedTransitionsDenominators[i][j] = 0.0;
                utterancesSums[i][j] = 0.0;
            }
        }


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
                    GMMLikelihoodFunction &stateGMM = this->hiddenStates->at(j);
                    forward[t][j] = 0.0; // initialize forward probability of being in state j in time t
                    for (int i = 0; i < states; i++) {
                        forward[t][j] += forward[t - 1][i] * this->transition[j][i];
                    }
                    double emission = stateGMM(utteranceIt->at(t));
                    forward[t][j] *= emission; // multiply by emission probability
                }
            }

            // recursively calculate the backward probabilites
            for (int i = 0; i < states; i++) {
                for (int t = T - 1; t >= 0; t--) {
                    backward[t][i] = 0.0;
                    for (int j = 0; j < states; j++) {
                        GMMLikelihoodFunction &stateGMM = this->hiddenStates->at(j);
                        backward[t][i] += this->transition[i][j] * backward[t + 1][j] *
                                          stateGMM(utteranceIt->at(t + 1));
                    }
                }
            }

            // calculate the state occupation probability
            for (int t = 0; t < times; t++) {
                for (int s = 0; s < states; s++) {
                    double occupationSum = EPS;
                    for (int s_prim = 0; s_prim < states; s_prim++) {
                        occupationSum += forward[t][s_prim] * backward[t][s_prim];
                    }

                    occupation[t][s] = forward[t][s] * backward[t][s] / occupationSum; // γ (s, t | word )
                }
            }

            for (int m = 0; m < this->M; m++) {
//            std::cout << "Occupation for " << m << "-th mixture:" << std::endl;
                for (int s = 0; s < states; s++) {
                    for (int t = 0; t < times; t++) {
                        valarray<double> &vector = utteranceIt->at(t);
                        GMMLikelihoodFunction &stateGMM = this->hiddenStates->at(s);
                        double probability = this->calculatePosteriorProbability(m, s, vector);
                        double weight = occupation[t][s] * probability;
                        valarray<double> difference = vector - stateGMM.getMeans(m);
                        valarray<double> product = difference * difference;

                        utterancesSums[s][m] += weight;
                        recalculatedMeans[s][m] += weight * vector;
                        recalculatedVariances[s][m] += weight * product;

                        std::cout <<
                        "m = " << m << ", " <<
                        "s = " << s << ", " <<
                        "t = " << t << ", " <<
                        "occupation[t][s] = " << occupation[t][s] << ", " <<
                        "probability = " << probability << ", " <<
                        "weight = " << weight << ", " <<
                        "difference = " << difference << ", " <<
                        "product = " << product << std::endl;
                    }

//                std::cout << std::endl;
                }
//            std::cout << std::endl;
            }

            for (int s1 = 0; s1 < states; s1++) {
                for (int s2 = 0; s2 < states; s2++) {
                    double numerator = 0.0;
                    double denominator = EPS;
                    for (int t = 0; t < times - 1; t++) { // last should be omitted
                        valarray<double> &vector = utteranceIt->at(t + 1);
                        numerator += this->calculatePosteriorTransitionProbability(s1, s2, t, forward, backward,
                                                                                   vector);
                        denominator += occupation[s1][t];
                    }

                    recalculatedTransitionsNumerators[s1][s2] += numerator;
                    recalculatedTransitionsDenominators[s1][s2] += denominator;
                }
            }

            for (int i = 0; i < times; i++) {
                delete[] forward[i];
                delete[] backward[i];
                delete[] occupation[i];
            }

            delete[] forward;
            delete[] backward;
            delete[] occupation;
        }

        for (int s = 0; s < this->states; s++) {
            for (int m = 0; m < this->M; m++) {
                valarray<double> newMeans = recalculatedMeans[s][m] / (utterancesSums[s][m] + EPS);
                valarray<double> newVariances = recalculatedVariances[s][m] / (utterancesSums[s][m] + EPS);

                GMMLikelihoodFunction &stateGMM = this->hiddenStates->at(s);
                stateGMM.setMeans(m, newMeans);
//                stateGMM.setVariances(m, newVariances);

                std::cout << "Means / variances for " << m << "-th mixture of " << s << " state:" << std::endl;
                for (int i = 0; i < newMeans.size(); i++) {
                    std::cout << newMeans[i] << '/' << newVariances[i] << ' ';
                }

                std::cout << std::endl;
            }
        }

        // recalculate transition matrix
        for (int s1 = 0; s1 < states; s1++) {
            for (int s2 = 0; s2 < states; s2++) {
                this->transition[s1][s2] =
                        recalculatedTransitionsNumerators[s1][s2] / recalculatedTransitionsDenominators[s1][s2];
            }
        }

        normalizeTransitionsMatrix();
        displayTransitionsMatrix();

        // TODO: delete recalculatedTransitionsNumerators and recalculatedTransitionsDenominators

        delete[] utterancesSums;
        delete[] recalculatedMeans;
    }

    // TODO: implement the Bauch-Welch algortihm for learning the coefficients

    // TODO: add the end all utterances can be removed, and model can be marked as fitted
}*/

void speech::HMMLexicon::MultivariateGaussianHMM::fit() {
    this->initialize(); // intialize HMM parameters

    unsigned int utterancesNumber = this->utterances->size();

    // TODO: number of iterations should be replaced with checking if the values converged
    for (int iteration = 0; iteration < 10; iteration++) {
        // display the intial probabilities and transition matrix
        this->displayPi();
        this->displayTransitionsMatrix();

        // declare structures for reestimated initial probabilities and transitions
        double *reestimatedPi = new double[this->states];
        double **reestimatedTransitionNumerators = new double *[this->states];
        double **reestimatedTransitionDenominators = new double *[this->states];
        for (int s1 = 0; s1 < this->states; s1++) {
            reestimatedTransitionNumerators[s1] = new double[this->states];
            reestimatedTransitionDenominators[s1] = new double[this->states];
            reestimatedPi[s1] = 0.0; // fill the array with zeros
            for (unsigned int s2 = 0; s2 < this->states; s2++) {
                reestimatedTransitionNumerators[s1][s2] = 0.0; // fill the array with zeros
                reestimatedTransitionDenominators[s1][s2] = 0.0; // fill the array with zeros
            }
        }

        // calculate posteriors for each utterance
        for (int u = 0; u < utterancesNumber; u++) {
            Observation &utterance = this->utterances->at(u);
            unsigned int times = utterance.size();

            // calculate forward and backward terms using Baum-Welch algortihm
            double **forward = this->calculateForwardTerms(utterance);
            double **backward = this->calculateBackwardTerms(utterance);

            // a posteriori probability given the utterance, that the process was in state s at time t
            double **posteriorOccupation = this->calculatePosteriorOccupationProbabilities(forward, backward,
                                                                                           utterance);

            // a posteriori probability given the utterance, that the process was in state s1 at time t,
            // and subsequently in state s2 at time t+1
            double ***posteriorTransition = this->calculatePosteriorTransitionProbabilities(forward, backward,
                                                                                            utterance);

            // calculate reestimated intial probabilities, transitions and state output density parameters
            for (int s = 0; s < this->states; s++) {
                reestimatedPi[s] += posteriorOccupation[s][0];
//                std::cout << "posteriorOccupation[" << s << "][0] = " << posteriorOccupation[s][0] << std::endl;
            }

            for (int s1 = 0; s1 < this->states; s1++) {
                double denominatorSum = EPS;
                for (int t = 0; t < times - 1; t++) {
                    denominatorSum += posteriorOccupation[s1][t];
                }

                for (int s2 = 0; s2 < this->states; s2++) {
                    double numeratorSum = 0.0;
                    for (int t = 0; t < times - 1; t++) {
                        numeratorSum += posteriorTransition[s1][s2][t];
                    }

                    reestimatedTransitionNumerators[s1][s2] = numeratorSum;
                    reestimatedTransitionDenominators[s1][s2] = denominatorSum;

                    std::cout << "numeratorSum = " << numeratorSum << std::endl;
                    std::cout << "denominatorSum = " << denominatorSum << std::endl;
                }
            }

            // remove created structures
            for (int s1 = 0; s1 < this->states; s1++) {
                delete[] forward[s1];
                delete[] backward[s1];
                delete[] posteriorOccupation[s1];
                for (unsigned int s2 = 0; s2 < this->states; s2++) {
                    delete[] posteriorTransition[s1][s2];
                }
                delete[] posteriorTransition[s1];
            }
            delete[] forward;
            delete[] backward;
            delete[] posteriorOccupation;
            delete[] posteriorTransition;
        }

        // reestimare model parameters
//        this->reestimateParameters();

        // replace the initial probabilities with new estimate
        delete[] this->pi;
        this->pi = reestimatedPi;
        this->normalizePiVector();

        // replace the transition with new estimate
        for (int s1 = 0; s1 < this->states; s1++) {
            for (int s2 = 0; s2 < this->states; s2++) {
                this->transition[s1][s2] =
                        reestimatedTransitionNumerators[s1][s2] / reestimatedTransitionDenominators[s1][s2];
            }

            delete[] reestimatedTransitionNumerators[s1];
            delete[] reestimatedTransitionDenominators[s1];
        }
        delete[] reestimatedTransitionNumerators;
        delete[] reestimatedTransitionDenominators;
        this->normalizeTransitionsMatrix();
    }
}

void speech::HMMLexicon::MultivariateGaussianHMM::initialize() {
    // initalize initial probabilities of each state
    for (int s = 0; s < this->states; s++) {
        this->pi[s] = 1.0 / this->states;
    }

    // initialize transition probabilities - only loopbacks or going to next state is possible
    srand(time(NULL));
    for (int s1 = 0; s1 < this->states; s1++) {
        for (int s2 = 0; s2 < this->states; s2++) {
            this->transition[s1][s2] = 0.0;
        }

        this->transition[s1][s1] = 0.5;
        if (s1 < this->states - 1) {
            this->transition[s1][s1 + 1] = 0.5;
        }
    }

    // normalize the transition matrix
    this->normalizeTransitionsMatrix();

    // initialize Gaussians
    for (int s = 0; s < this->states; s++) {
        for (int m = 0; m < this->M; m++) {
            for (int d = 0; d < this->dimensionality; d++) {
                GMMLikelihoodFunction &stateGMM = this->hiddenStates->at(s);
                stateGMM.setMean(m, d, (10.0 * rand() / RAND_MAX) - 5.0); // TODO: use data!
                stateGMM.setVariance(m, d,
                                     3.0);//5.0 + (5.0 * rand() / //RAND_MAX)); // TODO: remove this fix, variance should be much lower
            }
        }
    }
}

double **speech::HMMLexicon::MultivariateGaussianHMM::calculateForwardTerms(Observation & utterance) {
    unsigned int times = utterance.size();
    double **forward = new double *[this->states];
    for (int s = 0; s < this->states; s++) {
        forward[s] = new double[times];
    }

    // use intial probabilities for t = 0
    for (int s = 0; s < this->states; s++) {
        forward[s][0] = this->pi[s];
    }

    // calculate probabilities for all other states and times
    for (int s1 = 0; s1 < this->states; s1++) {
        GMMLikelihoodFunction &stateGMM = this->hiddenStates->at(s1);
        for (int t = 1; t < times; t++) {
            valarray <double> &vector = utterance[t];

            double sum = EPS;
            for (int s2 = 0; s2 < this->states; s2++) {
                sum += forward[s2][t - 1] * this->transition[s2][s1] * stateGMM(vector);
            }

            forward[s1][t] = sum;
        }
    }

    return forward;
}

double **speech::HMMLexicon::MultivariateGaussianHMM::calculateBackwardTerms(Observation & utterance) {
    int times = utterance.size();
    double **backward = new double *[this->states];
    for (int s = 0; s < this->states; s++) {
        backward[s] = new double[times];
    }

    // use probabilities of transition into end state for t = T_max
    int T = times - 1;
    int E = this->states - 1;
    for (int s = 0; s < this->states; s++) {
        backward[s][T] = this->transition[s][E];
    }

    // calculate probabilities for all other states and times
    for (int s1 = 0; s1 < this->states; s1++) {
        for (int t = T - 1; t >= 0; t--) {
            valarray <double> &vector = utterance[t + 1];

            double sum = EPS;
            for (int s2 = 0; s2 < this->states; s2++) {
                GMMLikelihoodFunction &stateGMM = this->hiddenStates->at(s2);
                sum += backward[s2][t + 1] * this->transition[s1][s2] * stateGMM(vector);
            }

            backward[s1][t] = sum;
        }
    }

    return backward;
}

double **speech::HMMLexicon::MultivariateGaussianHMM::calculatePosteriorOccupationProbabilities(double **forward,
                                                                                                double **backward,
                                                                                                Observation &utterance) {
    unsigned int times = utterance.size();
    double **gamma = new double *[this->states];
    for (unsigned int s = 0; s < this->states; s++) {
        gamma[s] = new double[times];
    }

    for (int s1 = 0; s1 < this->states; s1++) {
        for (int t = 0; t < times; t++) {
            double sum = EPS;
            for (int s2 = 0; s2 < this->states; s2++) {
                sum += forward[s2][t] * backward[s2][t];
            }

            gamma[s1][t] = (forward[s1][t] * backward[s1][t]) / sum;
        }
    }

    return gamma;
}

double ***speech::HMMLexicon::MultivariateGaussianHMM::calculatePosteriorTransitionProbabilities(double **forward,
                                                                                                 double **backward,
                                                                                                 Observation &utterance) {
    unsigned int times = utterance.size();
    double ***gamma = new double **[this->states];
    for (int s1 = 0; s1 < this->states; s1++) {
        gamma[s1] = new double *[this->states];
        for (int s2 = 0; s2 < this->states; s2++) {
            gamma[s1][s2] = new double[times];
        }
    }

    unsigned int T = times - 1;
    for (int s1 = 0; s1 < this->states; s1++) {
        for (int s2 = 0; s2 < this->states; s2++) {
            GMMLikelihoodFunction &stateGMM = this->hiddenStates->at(s2);

            for (int t = 0; t < T; t++) {
                valarray <double> &vector = utterance[t + 1];

                double sum = EPS;
                for (int s = 0; s < this->states; s++) {
                    sum += forward[s][t] * backward[s][t];
                }

                gamma[s1][s2][t] =
                        forward[s1][t] * this->transition[s1][s2] * stateGMM(vector) * backward[s2][t + 1] / sum;
            }
        }
    }

    return gamma;
}

double speech::HMMLexicon::MultivariateGaussianHMM::calculatePosteriorProbability(unsigned int m, unsigned int s,
                                                                                  const valarray <double> &vector) { // todo: check if it is calculated properly
    GMMLikelihoodFunction &stateGMM = this->hiddenStates->at(s);
    double P_s_k = 1.0;// / this->M; // TODO: it should be a probability P_s(k), but how to calculate it?

    double sum = 0.0;
    for (int j = 0; j < this->M; j++) {
        double P_s_j = 1.0;// / this->M; // TODO: it should be a probability P_s(j), but how to calculate it?
        sum += P_s_j * stateGMM(j, vector);
    }

    return P_s_k * stateGMM(m, vector) / (sum + EPS);
}

double speech::HMMLexicon::MultivariateGaussianHMM::calculatePosteriorTransitionProbability(unsigned int s1,
                                                                                            unsigned s2, unsigned t,
                                                                                            double **forward,
                                                                                            double **backward,
                                                                                            const valarray <double> &vector) {
    GMMLikelihoodFunction &stateGmm = this->hiddenStates->at(s2);
    double P = stateGmm(vector);
    double numerator = forward[s1][t] * transition[s1][s2] * P * backward[s2][t + 1];
    double denominator = EPS;
    for (int s = 0; s < this->states; s++) {
        denominator += forward[s][t] * backward[s][t];
    }

    return numerator / denominator;
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
    for (int s1 = 0; s1 < this->states; s1++) {
        double sum = 0.0;
        for (int s2 = 0; s2 < this->states; s2++) {
            sum += this->transition[s1][s2];
        }

        for (int s2 = 0; s2 < this->states; s2++) {
            this->transition[s1][s2] /= sum;
        }
    }
}

void speech::HMMLexicon::MultivariateGaussianHMM::normalizePiVector() { // TODO: check the normalization
    double sum = 0.0;
    for (int s = 0; s < this->states; s++) {
        sum += this->pi[s];
    }

    for (int s = 0; s < this->states; s++) {
        this->pi[s] /= sum;
    }
}