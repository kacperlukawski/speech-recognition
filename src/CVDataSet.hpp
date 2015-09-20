#ifndef CVDATASET_HPP
#define CVDATASET_HPP

#include <jsoncpp/json/json.h>
#include <vector>
#include <map>

/**
 * A collection of utterances to be used in a cross-validation
 * procedure. It is built from a collection of training and test
 * utterances.
 */
struct CVDataSet {
    /** Collection of utterances */
    typedef std::multimap<std::string, std::string> UtterancesSet;
    /** Single utterance file */
    typedef std::pair<std::string, std::string> UtterancePair;

    /** Utterances used in a training phase */
    UtterancesSet trainingFiles;
    /** Utterances used in a test phase */
    UtterancesSet testFiles;
    /** Model created using the training set */
    std::shared_ptr<speech::HMMLexicon> hmmLexicon;

    CVDataSet(const Json::Value &root) {
        const int SAMPLE_LENGTH = root["sample_length"].asInt(); // in milliseconds
        const int SAMPLE_OFFSET = root["sample_offset"].asInt(); // in milliseconds
        const int MFCC_BINS = root["mfcc_bins"].asInt(); // number of Mel filters in a bank
        const int MFCC_CEPSTRAL_COEFFICIENTS = root["mfcc_cepstral_coefficients"].asInt(); // number of cepstral coefficients used for analysis
        const double MFCC_MIN_FREQUENCY = root["mfcc_min_frequency"].asDouble(); // in Herzs
        const double MFCC_MAX_FREQUENCY = root["mfcc_max_frequency"].asDouble(); // in Herzs
        const int LEXICON_DIMENSIONALITY = 3 * (MFCC_CEPSTRAL_COEFFICIENTS + 1);
        const int LEXICON_GAUSSIANS = root["lexicon_gaussians"].asInt();
        const unsigned int MAX_ITERATIONS = root["max_iterations"].asInt();

        std::shared_ptr<AbstractGaussianInitializer> gaussianInitializer(new KMeansInitializer());
        this->hmmLexicon = std::shared_ptr<speech::HMMLexicon>(
                new speech::HMMLexicon(LEXICON_DIMENSIONALITY, LEXICON_GAUSSIANS, gaussianInitializer, MAX_ITERATIONS));
    }

    inline void addTrainingUtterance(std::string utterance, std::string file) {
        this->trainingFiles.insert(UtterancePair(utterance, file));
    }

    inline void addTestUtterance(std::string utterance, std::string file) {
        this->testFiles.insert(UtterancePair(utterance, file));
    }


};

/**
 * Divides the files into separate datasets used for cross validation. If the file is used to build
 * the model, then it is never used to test it. Each word has a one test file (1-out-of-N validation).
 * @param words a structure of JSON file with the model configuration
 * @param K number of created datasets
 * @return collection of datasets used for a cross validation of the model
 */
std::vector<CVDataSet> divideForCrossValidation(const Json::Value &root, int K) {
    std::vector<CVDataSet> cvDatasets;
    for (int k = 0; k < K; ++k) {
        // Create k datasets
        cvDatasets.push_back(CVDataSet(root));
        CVDataSet &dataset = cvDatasets.at(k);

        // Select vectors for the training and test phase
        const Json::Value words = root["words"];
        for (int i = 0; i < words.size(); i++) {
            const Json::Value word = words[i];
            std::string spelling = word["spelling"].asString();
            std::string transcription = word["transcription"].asString();

            const Json::Value utterances = word["utterances"];
            int utterancesCount = utterances.size();
            for (int j = 0; j < utterancesCount; j++) {
                std::string fileName = utterances[j].asString();
                if (k % utterancesCount == j) {
                    dataset.addTestUtterance(transcription, fileName);
                } else {
                    dataset.addTrainingUtterance(transcription, fileName);
                }
            }
        }
    }

    return std::move(cvDatasets);
}

#endif