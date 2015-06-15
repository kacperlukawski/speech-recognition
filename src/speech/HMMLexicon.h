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

namespace speech {

    /**
     * An acoustic model of the language built from HMMs for each
     * word / syllabe / phoneme from the dictionary.
     */
    class HMMLexicon {
    public:
        typedef vector<valarray<double>> Observation;

        /**
         * Constructs an object
         * @param dimensionality dimensionality of a single acoustic vector
         */
        HMMLexicon(int dimensionality);

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
        void addUtterance(const Observation &utterance,
                          string transcription,
                          string unitSeparator = "|");

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

//    protected:
//    TODO: make fields private / protected
//    private:
        class MultivariateGaussianHMM;

        /** Dimensionality of a single data vector */
        int dimensionality;
        /** Number of mixture Gaussians used for the probability approximation */
        static const unsigned int gaussians = 1;
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

        /**
         * Init a single row of transitions matrix randomly. The sum of transitions in a single
         * row sum up to 1 (so the overall probability is correct).
         * @param transitions pointer to the array containing the transitions row
         * @param number of states in the array
         */
        inline static void initTransitionsRandomly(double *transitions, int states) {
            double sum = 0.0;
            for (int i = 0; i < states; i++) {
                transitions[i] = (double) random();
                sum += transitions[i];
            }

            for (int i = 0; i < states; i++) {
                transitions[i] /= sum;
            }
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
            explicit GMMLikelihoodFunction(unsigned int M, unsigned int D);

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

            const double &getWeight(unsigned int k) const {
                return weights[k];
            }

            const valarray<double> &getMeans(unsigned int k) const {
                return means[k];
            }

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
            /** Minimal variance of each dimension */
            static constexpr double MIN_VARIANCE = 1e-10;
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
             */
            explicit MultivariateGaussianHMM(unsigned int dimensionality, unsigned int states, unsigned int M);

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
             * Calculates a likelihood that given utterance is a language unit
             * described by this model.
             * @param utterance observed sequence of vectors
             * @return likelihood of being this language word
             */
            double predict(const Observation &utterance);

        protected:
            /** Dimensionality of a single input vector */
            unsigned int dimensionality;
            /** Number of hidden states (language units, like phones) */
            unsigned int states;
            /** Number of Gaussians used to approximate the distribution */
            unsigned int M;
            /** Collection of observed vectors of this language unit */
            vector<Observation> *utterances;
            /** Probability distributions of the emiting hidden states */
            vector<GMMLikelihoodFunction> *hiddenStates;
            /** Initial states probability */
            double *pi;
            /** Probabilities of transition from state X to Y */
            double **transition;

            /**
             * Initializes HMM parameters
             */
            void initialize();

            /**
             * Calculates forward terms of Baum-Welch algorithm
             * @param utterance
             * @return this->states * times array containting terms
             */
            double **calculateForwardTerms(Observation &utterance);

            /**
             * Calculates backward terms of Baum-Welch algorithm
             * @param utterance
             * @return this->states * times array containting terms
             */
            double **calculateBackwardTerms(Observation &utterance);

            /**
             * Calculates state s in time t occupation probabilities
             * @param backward
             * @param forward
             * @param utterance
             * @return this->states * times array containing the probabilities
             */
            double **calculatePosteriorOccupationProbabilities(double **forward, double **backward,
                                                               Observation &utterance);

            /**
             * Calculates a posteriori probability given the utterance, that the process was in state s1 at time t,
             * and subsequently in state s2 at time t+1
             * @param backward
             * @param forward
             * @param utterance
             * @return this->states * this->states * times array containing the probabilities
             */
            double ***calculatePosteriorTransitionProbabilities(double **forward, double **backward,
                                                                Observation &utterance);

            /**
             * Calculates a posterior probability of m-th Gaussian in state s,
             * given observation vector.
             * @param m
             * @param s
             * @param vector
             * @return probability
             */
            inline double calculatePosteriorProbability(unsigned int m, unsigned int s, const valarray<double> &vector);

            /**
             * @param vector an observed acoustic vector in (t + 1) time
             */
            inline double calculatePosteriorTransitionProbability(unsigned int s1, unsigned s2, unsigned t,
                                                                  double **forward, double **backward,
                                                                  const valarray<double> &vector);

            /**
             * Displays the transition matrix
             */
            void displayTransitionsMatrix() const;

            /**
             * Displays the intial probabilities vector
             */
            void displayPi() const;

            /**
             * Normalizes a transition matrix
             */
            void normalizeTransitionsMatrix();

            /**
             * Normalizes a initial probabilities vector
             */
            void normalizePiVector();
        };
    };

}

#endif //SPEECH_RECOGNITION_HMMLEXICON_H
