#include "HMM.h"

//
// @todo implement loading the model
//
speech::spelling::HMM::HMM(std::istream &in) {
    in.read((char *) &numberOfObservations, sizeof(numberOfObservations));
    in.read((char *) &numberOfStates, sizeof(numberOfStates));

    observations = new std::vector<int>(numberOfObservations);
    states = new std::vector<char>(numberOfStates);

    for (int i = 0; i < numberOfObservations; i++) {
        in.read((char *) &observations->at(i), sizeof(int));
    }

    for (int i = 0; i < numberOfStates; i++) {
        in.read((char *) &states->at(i), sizeof(char));
    }

    transmission = new arma::mat(numberOfStates, numberOfStates); // at(fromState, toState)
    emission = new arma::mat(numberOfStates, numberOfObservations); // at(state, observation)
    pi = new arma::vec(numberOfStates);

    in.read((char *) transmission->memptr(), transmission->n_elem * sizeof(double));
    in.read((char *) emission->memptr(), emission->n_elem * sizeof(double));

    for (int i = 0; i < numberOfStates; i++) {
        in.read((char *) &pi->at(i), sizeof(double));
    }

    stateToStateCount = new arma::mat(numberOfStates, numberOfStates);
    stateToObservationCount = new arma::mat(numberOfStates, numberOfObservations);
    stateCount = new arma::vec(numberOfStates);

    in.read((char *) stateToObservationCount->memptr(), stateToObservationCount->n_elem * sizeof(double));
    in.read((char *) stateToStateCount->memptr(), stateToStateCount->n_elem * sizeof(double));

    for (int i = 0; i < numberOfStates; i++) {
        in.read((char *) &stateCount->at(i), sizeof(double));
    }
}

speech::spelling::HMM::HMM(int _numberOfObservations, int _numberOfStates)
        : numberOfObservations(_numberOfObservations), numberOfStates(_numberOfStates) {
    observations = new std::vector<int>(numberOfObservations);
    for (unsigned int i = 0; i < numberOfObservations; ++i) {
        observations->at(i) = i;
    }

    states = new std::vector<char>(numberOfStates);
    for (unsigned int i = 0; i < numberOfStates; ++i) {
        states->at(i) = 'a' + (char) i;
    }

    transmission = new arma::mat(numberOfStates, numberOfStates); // at(fromState, toState)
    emission = new arma::mat(numberOfStates, numberOfObservations); // at(state, observation)
    pi = new arma::vec(numberOfStates);

    transmission->zeros();
    emission->zeros();

    stateToStateCount = new arma::mat(numberOfStates, numberOfStates);
    stateToObservationCount = new arma::mat(numberOfStates, numberOfObservations);
    stateCount = new arma::vec(numberOfStates);

    stateToStateCount->zeros();
    stateToObservationCount->zeros();
}

speech::spelling::HMM::HMM(const std::vector<int> &_observations, const std::vector<char> &_states)
        : HMM(_observations.size(), _states.size()) {
    delete observations;
    delete states;

    observations = new std::vector<int>(_observations);
    states = new std::vector<char>(_states);
}

speech::spelling::HMM::HMM(int _numberOfObservations, const std::vector<char> &_states)
        : HMM(_numberOfObservations, _states.size()) {
    delete states;

    states = new std::vector<char>(_states);
}

speech::spelling::HMM::~HMM() {
    delete observations;
    delete states;
    delete transmission;
    delete emission;
    delete pi;
    delete stateToStateCount;
    delete stateToObservationCount;
    delete stateCount;
}

/**
* Add next example of the training data - connections
* between sequence of phonems and sequence of letters.
*/
void speech::spelling::HMM::fit(std::vector<int> &phonems, const std::string &spelling) {
    if (phonems.size() != spelling.length()) {
        // @todo it is necessary to adjust the spelling somehow by duplicating some of the letters - some phonems last longer than another
        return;
    }

    // increase the number of the starting states
    int startStateIndex = stateToIndex(spelling[0]);
    stateCount->at(startStateIndex) = stateCount->at(startStateIndex) + 1.0;

    // increase number of emissions
    int stateIndex = 0;
    int observationIndex = 0;
    for (int i = 0; i < phonems.size(); ++i) {
        stateIndex = stateToIndex(spelling[i]);
        observationIndex = observationToIndex(phonems[i]);
        stateToObservationCount->at(stateIndex, observationIndex) = stateToObservationCount->at(stateIndex, observationIndex) + 1.0;
    }

    // increase number of transmissions
    int firstStateIndex = 0;
    int secondStateIndex = 0;
    for (int i = 0; i < spelling.length() - 1; ++i) {
        firstStateIndex = stateToIndex(spelling[i]);
        secondStateIndex = stateToIndex(spelling[i + 1]);
        stateToStateCount->at(firstStateIndex, secondStateIndex) = stateToStateCount->at(firstStateIndex, secondStateIndex) + 1.0;
    }

    actualizeProbabilityDistributions();
}

/**
* Perform a Viterbi algorithm to find most likely
* sequence of states which led to given observation.
* Phonems are considered to be observations, while
* letters of an alphabet are hidden states.
*
* @return sequence of letters
*/
std::string speech::spelling::HMM::predict(std::vector<int> phonems) {
    arma::mat *probabilities = new arma::mat(phonems.size(), numberOfStates);
    probabilities->zeros();

    std::vector<std::string> *statesPaths = new std::vector<std::string>(numberOfStates);

    char stateLabel = 0;
    int stateLabelIndex = 0;
    int observationIndex = 0;
    for (auto it = states->begin(); it != states->end(); ++it) {
        stateLabel = *it;
        stateLabelIndex = stateToIndex(stateLabel);
        observationIndex = observationToIndex(phonems[0]);

        statesPaths->at(stateLabelIndex) = std::string("");
        statesPaths->at(stateLabelIndex).push_back(stateLabel);

        probabilities->at(0, stateLabelIndex) = pi->at(stateLabelIndex) * emission->at(stateLabelIndex, observationIndex) + EPS;
    }

    char startStateLabel = 0;
    int startStateLabelIndex = 0;
    for (int i = 1; i < phonems.size(); ++i) {
        std::vector<std::string> *newStatesPaths = new std::vector<std::string>(numberOfStates);

        observationIndex = observationToIndex(phonems[i]);
        for (auto it = states->begin(); it != states->end(); ++it) {
            stateLabel = *it;
            stateLabelIndex = stateToIndex(stateLabel);

            // select a start state which maximizes
            // the probability of getting there
            double maxProbability = 0.0;
            int maxProbabilityStateLabelIndex = 0;
            for (auto startIt = states->begin(); startIt != states->end(); ++startIt) {
                startStateLabel = *startIt;
                startStateLabelIndex = stateToIndex(startStateLabel);

                double currentProbability =
                                probabilities->at(i - 1, startStateLabelIndex) *
                                transmission->at(startStateLabelIndex, stateLabelIndex) *
                                emission->at(stateLabelIndex, observationIndex) + EPS;

                if (currentProbability <= maxProbability) {
                    continue;
                }

                maxProbability = currentProbability;
                maxProbabilityStateLabelIndex = startStateLabelIndex;
            }

            // store the path which led us here
            std::string newPath(statesPaths->at(maxProbabilityStateLabelIndex));
            newPath.push_back(stateLabel);
            newStatesPaths->at(stateLabelIndex) = newPath;
            probabilities->at(i, stateLabelIndex) = maxProbability;
        }

        delete statesPaths;
        statesPaths = newStatesPaths;
    }

    // find the most probable path by getting the maximum value
    int maxProbabilityIndex = 0;
    for (int i = 0; i < numberOfStates; ++i) {
        std::string current(statesPaths->at(maxProbabilityIndex));
        if (probabilities->at(phonems.size() - 1, i) > probabilities->at(phonems.size() - 1, maxProbabilityIndex)) {
            maxProbabilityIndex = i;
        }
    }

    std::string maxProbableStatesSequence(statesPaths->at(maxProbabilityIndex));

    delete probabilities;
    delete statesPaths;

    return maxProbableStatesSequence;
}

/**
* Convert given character into index taken from
* the states collection.
*
* @todo add throwing an exception when the state is not present in the set of states
*/
int speech::spelling::HMM::stateToIndex(const char &state) {
    auto elementPosIt = std::find(states->begin(), states->end(), state);
    if (elementPosIt == states->end()) {
        return 0;
    }

    return (int) std::distance(states->begin(), elementPosIt);
}

/**
* Convert given integer identifier of an observation
* into internal index. It will be mostly the same,
* but in some cases it is necessary to label the data
* another way.
*
* @todo add throwing an exception when observation is not present in observations set
*/
int speech::spelling::HMM::observationToIndex(const int &observation) {
    auto elementPosIt = std::find(observations->begin(), observations->end(), observation);
    if (elementPosIt == observations->end()) {
        return 0;
    }

    return (int) std::distance(observations->begin(), elementPosIt);
}

/**
* Create matrices of probabilities by normalization
*
* @todo check if the provided algorithm is correct (rows are not used when the cols should be)
*/
void speech::spelling::HMM::actualizeProbabilityDistributions() {
    for (int i = 0; i < numberOfStates; ++i) {
        pi->at(i) = stateCount->at(i) / (sum(*stateCount) + EPS);
    }

    for (int i = 0; i < numberOfObservations; ++i) {
        emission->unsafe_col(i) = stateToObservationCount->unsafe_col(i) / (sum(stateToObservationCount->unsafe_col(i)) + EPS);
    }

    for (int i = 0; i < numberOfObservations; ++i) {
        transmission->unsafe_col(i) = stateToStateCount->unsafe_col(i) / (sum(stateToStateCount->unsafe_col(i)) + EPS);
    }
}

//
// @todo implement the serialization
//
void speech::spelling::HMM::serialize(std::ostream &out) const {
    uint32_t typeIdentifier = TYPE_IDENTIFIER;

    out.write((char const *) &typeIdentifier, sizeof(typeIdentifier));
    out.write((char const *) &numberOfObservations, sizeof(numberOfObservations));
    out.write((char const *) &numberOfStates, sizeof(numberOfStates));

    for (int i = 0; i < numberOfObservations; i++) {
        out.write((char const *) &observations->at(i), sizeof(int));
    }

    for (int i = 0; i < numberOfStates; i++) {
        out.write((char const *) &states->at(i), sizeof(char));
    }

    out.write((char const *) transmission->memptr(), transmission->n_elem * sizeof(double));
    out.write((char const *) emission->memptr(), emission->n_elem * sizeof(double));

    for (int i = 0; i < numberOfStates; i++) {
//    arma::vec *pi; // initial state distribution
        out.write((char const *) &pi->at(i), sizeof(double));
    }

    out.write((char const *) stateToObservationCount->memptr(), stateToObservationCount->n_elem * sizeof(double));
    out.write((char const *) stateToStateCount->memptr(), stateToStateCount->n_elem * sizeof(double));

    for (int i = 0; i < numberOfStates; i++) {
        //    arma::vec *stateCount; // numbers of occurences of each state as a first state in a sequence
        out.write((char const *) &stateCount->at(i), sizeof(double));
    }
}
