#ifndef HMM_H
#define HMM_H

#include "ISpellingTranscription.h"

#include <armadillo>
#include <valarray>
#include <string>

namespace speech {

    namespace spelling {

        class HMM : public ISpellingTranscription {
        public:
            static const uint32_t TYPE_IDENTIFIER = 0x01000001;
            
            HMM(int _numberOfObservations, int _numberOfStates);
            
            HMM(std::istream& in);

            HMM(const std::vector<int>& _observations, const std::vector<char>& _states);

            HMM(int _numberOfObservations, const std::vector<char>& _states);

            virtual ~HMM();

            virtual void fit(std::vector<int> &phonems, const std::string &spelling);

            virtual std::string predict(std::vector<int> phonems);

            virtual void serialize(std::ostream &out) const;
        protected:
            int numberOfObservations; // M
            int numberOfStates; // N
            std::vector<int>* observations; // list of possible observations (phonems)
            std::vector<char>* states; // list of possible states (letters)
            arma::mat *transmission; // state transition probability distribution - transmission matrix
            arma::mat *emission; // observation symbol probability distribution - emission matrix
            arma::vec *pi; // initial state distribution
        private:
            const double EPS = 10e-7;
            arma::mat *stateToObservationCount; // numbers of emissions between each state and observation
            arma::mat *stateToStateCount; // numbers of transmitions between each pair of states
            arma::vec *stateCount; // numbers of occurences of each state as a first state in a sequence

            inline int stateToIndex(const char &state);

            inline int observationToIndex(const int &observation);

            void actualizeProbabilityDistributions();
        };

    }

}

#endif