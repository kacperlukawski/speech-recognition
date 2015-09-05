//
// Created by kacper on 04.06.15.
//

#ifndef SPEECH_RECOGNITION_HMMLEXICON_H
#define SPEECH_RECOGNITION_HMMLEXICON_H

#include <armadillo>

using arma::mat;
using arma::vec;

#include <stdexcept>
#include <valarray>
#include <vector>
#include <string>
#include <map>

using std::valarray;
using std::vector;
using std::string;
using std::map;

#include "raw_data/DataSource.h"

using speech::raw_data::DataSource;

#include "operators.h"
#include "helpers.h"

namespace speech {

    namespace initializer {
        class AbstractGaussianInitializer;
    }

    /**
     * An acoustic model of the language built from HMMs for each
     * word / syllabe / phoneme from the dictionary.
     */
    class HMMLexicon {
    public:
        typedef vector<valarray<double>> Observation;

        /**
         * Constructs an object with a custom initializer
         * @param dimensionality dimensionality of a single acoustic vector
         * @param gaussians number of Gaussian mixtures in a single state of HMM
         * @param initializer an instance of custom initializer
         * @param maxIterations a maximum number of iterations to perform in model fitting
         */
        HMMLexicon(int dimensionality, unsigned int gaussians,
                   std::shared_ptr<speech::initializer::AbstractGaussianInitializer> initializer,
                   unsigned int maxIterations = 100);

        /**
         * Destructs the object
         */
        virtual ~HMMLexicon();

        /**
         * Adds an acoustic observation of given text
         * @param utterance collection of acoustic observation vectors
         * @param transcription text representing the observation
         * @param unitSeparator separator used for splitting the transcription
         */
        void addUtterance(Observation utterance, string transcription, string unitSeparator = "|");

        /**
         * Predicts a word which maximizes the probability of being
         * observed in given observation
         * @param observation collection of acoustic observation vectors
         * @return a language unit maximizing the probability of being observed
         */
        string predict(const Observation &observation);

        /**
         * Fits the parameters of the lexicon
         */
        void fit();

        /**
         * Gets number of the unit models
         * @return number of HMMs
         */
        inline unsigned long size() {
            return unitModels.size();
        }

        /**
         * Function of a likelihood approximating the distribution
         * using Gaussian mixture model.
         */
        class GMMLikelihoodFunction {
        public:
            /**
             * Constructs an object representing function built from given
             * number of multivariate Gaussians.
             * @param M number of Gaussians
             * @param D dimensionality of a single input vector
             */
            GMMLikelihoodFunction(unsigned int M, unsigned int D);

            /**
             * Calculates the probability of being in the state modelled
             * by this likelihood function, when given vector was observed.
             * @param observation observed acoustic vector
             * @return probability of being in this state
             * @throws std::out_of_range when given vector size is wrong
             */
            double operator()(const valarray<double> &observation);

            /**
             * Calculates the probability of being in the state modelled
             * by the k-th component of this likelihood function, when given vector was observed.
             * @param k mixture
             * @param observation observed acoustic vector
             * @return probability of being in this state
             * @throws std::out_of_range when given vector size is wrong
             */
            double operator()(unsigned int k, const valarray<double> &observation);

            /**
             * Gets a weight of selected mixture
             * @param k mixture number
             */
            const double &getWeight(unsigned int k) const {
                return weights[k];
            }

            /**
             * Get means of selected mixture
             * @param k mixture number
             */
            const valarray<double> &getMeans(unsigned int k) const {
                return means[k];
            }

            /**
             * Get variances of selected mixture
             * @param k mixture number
             */
            const valarray<double> &getVariances(unsigned int k) const {
                return variances[k];
            }

            /**
             * Sets new weight of the selected Gaussian mixture
             * @param mixture
             * @param weight new weight
             * @throws std::out_of_range when selected dimension does not exist
             */
            inline void setWeight(int mixture, double weight) {
                if (mixture < 0 || mixture >= this->M) {
                    throw std::out_of_range("Given mixture is wrong");
                }

                this->weights[mixture] = weight;
            }

            /**
             * Sets new mean of the selected Gaussian mixture and dimension
             * @param mixture
             * @param dimension
             * @param mean new mean
             * @throws std::out_of_range when selected dimension does not exist
             */
            inline void setMean(int mixture, int dimension, double mean) {
                if (mixture < 0 || mixture >= this->M) {
                    throw std::out_of_range("Given mixture is wrong");
                }

                if (dimension < 0 || dimension >= this->D) {
                    throw std::out_of_range("Given dimension is wrong");
                }

                this->means[mixture][dimension] = mean;
            }

            /**
             * Sets new means of the selected Gaussian mixture
             * @param mixture
             * @param means new means
             * @throws std::out_of_range when given vector has wrong size
             */
            inline void setMeans(int mixture, valarray<double> means) {
                if (means.size() != this->D) {
                    throw std::out_of_range("Wrong size of given means vector");
                }

                this->means[mixture] = means;
            }

            /**
             * Sets new variance of the selected Gaussian mixture and dimension
             * @param mixture
             * @param dimension
             * @param variance new variance
             * @throws std::out_of_range when selected dimension does not exist
             */
            inline void setVariance(int mixture, int dimension, double variance) {
                if (mixture < 0 || mixture >= this->M) {
                    throw std::out_of_range("Given mixture is wrong");
                }

                if (dimension < 0 || dimension >= this->D) {
                    throw std::out_of_range("Given dimension is wrong");
                }

                this->variances[mixture][dimension] = variance;
            }

            /**
             * Sets new variances of the selected Gaussian mixture
             * @param mixture
             * @param variances new variances
             * @throws std::out_of_range when selected dimension does not exist
             */
            inline void setVariances(int mixture, valarray<double> variances) {
                if (variances.size() != this->D) {
                    throw std::out_of_range("Wrong size of given variance vector");
                }

                this->variances[mixture] = variances;
            }

        protected:
            /** Number of mixtures */
            unsigned int M;
            /** Dimensionality of an acoustic input vector */
            unsigned int D;
            /** Mixtures' weights (M) */
            valarray<double> weights;
            /** Means of mixtures (M x D) */
            vector<valarray<double>> means;
            /** Variances of mixtures (M x D) (could be matrices of covariances - M x (D x D)) */
            vector<valarray<double>> variances;
        };

        /**
         * A continuous HMM aproximating the likelihood of being in a partcular
         * state with multivariate Gaussians.
         */
        class MultivariateGaussianHMM {
        public:
            /**
             * Construct an HMM model with given number of observations and hidden states.
             * The probability of being in a particular state at the fixed time is approximated
             * using Gaussian mixtures.
             * @param dimensionality size of a input vector
             * @param states number of hidden states
             * @param M number of Gaussian in each GMM
             * @param initializer an instance of the Gaussian initializer
             */
            MultivariateGaussianHMM(unsigned int dimensionality, unsigned int states, unsigned int M,
                                    std::shared_ptr<speech::initializer::AbstractGaussianInitializer> initializer,
                                    unsigned int maxIterations);

            /**
             * Destructs the object
             */
            virtual ~MultivariateGaussianHMM();

            /**
             * Adds an observation of the language unit that this HMM is created for
             * @param utterance observed sequence of vectors
             */
            void addUtterance(const Observation &utterance);

            /**
             * Fits the model using set of previously provided observations.
             * Fitting process is done using forward-backward Baum-Welch algortihm
             */
            void fit();

            /**
             * Calculates a log-likelihood that given utterance is a language unit
             * described by this model.
             * @param utterance observed sequence of vectors
             * @return likelihood of being this language word
             */
            double predict(const Observation &utterance);

            inline unsigned int getDimensionality() const {
                return this->dimensionality;
            }

            inline unsigned int getStates() const {
                return this->states;
            }

            inline unsigned int getNumberOfMixtures() const {
                return this->M;
            }

            inline vector<Observation> *getUtterances() const {
                return this->utterances;
            }

            inline GMMLikelihoodFunction& getHiddenState(int n) {
                return this->hiddenStates->at(n);
            }

        protected:
            /** Low value used to avoid mathematical errors */
            static constexpr double EPS = 1e-256;
            /** Minimal variance of a single dimension in the Gaussian mixture */
            static constexpr double MIN_VARIANCE = 1e-256;
            /** Dimensionality of a single input vector */
            unsigned int dimensionality;
            /** Number of hidden states (language units, like phones) */
            unsigned int states;
            /** Number of Gaussians used to approximate the distribution */
            unsigned int M;
            /** Initializer of the Gaussians */
            std::shared_ptr<speech::initializer::AbstractGaussianInitializer> initializer;
            /** Max iterations in a fitting phase */
            unsigned int maxIterations;
            /** Collection of observed vectors of this language unit */
            vector<Observation> *utterances;
            /** Probability distributions of the emiting hidden states */
            vector<GMMLikelihoodFunction> *hiddenStates;
            /** Initial states probability */
            double *pi;
            /** Probabilities of transition from state X to Y */
            double **transition;
            /** Determines if a model has been already fitted */
            bool fitted = false;

            /**
             * Initializes the mixtures using random values, but based on the dataset
             */
            void initializeMixtures();

            /**
             * Initializes model configuration (initial probabilities and transitions matrix)
             */
            void initializeModel();

            /**
             * Displays Gaussians
             */
            void displayHiddenStates() const;

            /**
             * Displays the transition matrix
             */
            void displayTransitionsMatrix() const;

            /**
             * Displays the intial probabilities vector
             */
            void displayPi() const;

            /**
             * Normalizes transitions matrix. Numbers calculated by the Baum-Welch
             * algorithm are just expected numbers of transitions from state i to state j
             * relative to the expected total number of transitions away from state i.
             */
            void normalizeTransitionsMatrix();

            /**
             * Normalizes a vector of initial probabilities of being in different states
             */
            void normalizePi();

        private:
            // TODO: comment the methood
            double calculateLogLikelihood(const Observation& observation);

            /**
             * Calculates forward probabilities of each vector from the observation and each state of HMM
             * @param forward an array storing the values of forward probabilities
             * @param observation collection of observed vectors
             * @param normalize determines if each time frame should have the probabilities summing up to 1
             */
            void calculateForwardProbabilities(double **forward, const Observation &observation, bool normalize = false);

            /**
             * Calculates backward probabilities of each vector from the observation and each state of HMM
             * @param backward an array storing the values of backward probabilities
             * @param observation collection of observed vectors
             * @param normalize determines if each time frame should have the probabilities summing up to 1
             */
            void calculateBackwardProbabilities(double **backward, const Observation &observation, bool normalize = false);

            /**
             * Calculates the probabilities of being in each state in each time frame
             * @param occcupation an array where store the probabilities in
             * @param forward forward probablities
             * @param backward backward probabilities
             * @param observation collection of observed vectors
             */
            void calculateOccupationEstimates(double ***occupation, double **forward, double **backward,
                                              const Observation &observation);

            /**
             * Calculates the probabilties of transition from each state into another one
             * @param transitions an array where store the probabilities in
             * @param occcupation state occupation probabilities
             * @param forward forward probablities
             * @param backward backward probabilities
             * @param observation collection of observed vectors
             */
            void calculateTransitionEstimates(double ***transitions, double **forward, double **backward,
                                              double ***occupation, const Observation &observation);

            /**
             * Accumulates the initial probabilities of being in each state
             * @param initialAcc an array where store the probabilities in
             * @param occupation state occupation probabilities
             */
            void accumulateInitialProbabilities(double *initialAcc, double ***occupation);

            /**
             * Accumulates the transition probabilities from state to state, without taking care of the time frames.
             * @param transitionsAcc an array where store the probabilities in
             * @param transitions array of transitions probabilities in time frames
             * @param observation collection of observed vectors
             */
            void accumulateTransitions(double **transitionsAcc, double ***transitions, const Observation &observation);

            /**
             * Accumulates the mixture occupation probabilities
             * @param occupationsAcc an array where store the probabilities in
             * @param weightedVarianceAcc an array where store the weighted variance of mixtures
             * @param occupation state occupation probabilities
             * @param observation collection of observed vectors
             */
            void accumulateMixtureOccupations(double **occupationsAcc, double ***occupation,
                                              const Observation &observation);

            /**
             * Accumulates new means of mixtures
             * @param weightedObservationAcc an array where store the weighted means of mixtures
             * @param occupation state occupation probabilities with time frames
             * @param occupationsAcc state occupation probabilities
             * @param observation collection of observed vectors
             */
            void accumulateMixtureMeans(valarray<double> **weightedObservationAcc, double ***occupation,
                                        double **occupationsAcc, const Observation &observation);


            // TODO: write doc
            void accumulateMixtureVariances(valarray<double> **weightedVarianceAcc,
                                            valarray<double> **weightedObservationAcc, double ***occupation,
                                            double **occupationsAcc, const Observation &observation);

            /**
             * Updates the parameters of Gaussian mixtures
             * @param occupationsAcc accumulated occupation probabilities
             * @param weightedObservationAcc weighted means of the observation using the importance of the mixtures
             * @param weightedVarianceAcc weighted variance using the importance of the mixtures
             */
            void updateMixtures(double **occupationsAcc, valarray<double> **weightedObservationAcc,
                                valarray<double> **weightedVarianceAcc);

        };

    protected:
        /** Dimensionality of a single data vector */
        int dimensionality;
        /** Number of mixture Gaussians used for the probability approximation */
        unsigned int gaussians;
        /** Initializer of the Gaussian mixtures */
        std::shared_ptr<speech::initializer::AbstractGaussianInitializer> initializer;
        /** Max iterations in a fitting phase */
        unsigned int maxIterations;
        /** Collection of Hidden Markov Models, each one represents a single language unit */
        map<string, MultivariateGaussianHMM *> unitModels;

        /**
         * Splits given string using given separator
         * @param text
         * @param separator
         * @return list of chunks
         */
        inline vector<string> split(const string &text, string &separator) {
            vector<string> chunks;
            unsigned long pos = 0;
            unsigned long endPosition = 0;

            while (true) {
                endPosition = text.find(separator, pos);
                if (string::npos == endPosition) {
                    break;
                }

                string chunk = text.substr(pos, endPosition - pos);
                if (chunk.length() > 0) {
                    chunks.push_back(chunk);
                }

                pos = endPosition + separator.length();
            }

            unsigned long length = text.length();
            if (pos < length) {
                string chunk = text.substr(pos, length - pos);
                chunks.push_back(chunk);
            }

            return std::move(chunks);
        }

    };

}

#include "initializer/AbstractGaussianInitializer.h"

#endif //SPEECH_RECOGNITION_HMMLEXICON_H
