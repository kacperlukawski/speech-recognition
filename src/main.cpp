#include <iostream>
#include <memory>
#include <list>
#include <streambuf>
#include <fstream>

using std::shared_ptr;

#include "speech/raw_data/WaveFileDataSource.h"

using speech::raw_data::WaveFileDataSource;

#include "speech/raw_data/DataSample.h"

#include "speech/transform/IFrequencyTransform.h"

#include "speech/clustering/KMeans.h"
#include "speech/clustering/GaussianMixtureModel.h"

using speech::clustering::IClusteringMethod;
using speech::clustering::KMeans;
using speech::clustering::GaussianMixtureModel;

#include "speech/spelling/HMM.h"

using speech::spelling::ISpellingTranscription;
using speech::spelling::HMM;

#include <vector>

#include "speech/LanguageModel.h"

using speech::LanguageModel;

#include "speech/clustering/exception/TooLessVectorsException.h"

using namespace speech::transform;

#include "speech/vectorizer/IVectorizer.h"
#include "speech/vectorizer/MaxFrequencyVectorizer.h"
#include "speech/vectorizer/FormantVectorizer.h"
#include "speech/vectorizer/ThirdsPowerVectorizer.h"
#include "speech/vectorizer/MFCCVectorizer.h"

using speech::vectorizer::IVectorizer;
using speech::vectorizer::MaxFrequencyVectorizer;
using speech::vectorizer::FormantVectorizer;
using speech::vectorizer::ThirdsPowerVectorizer;
using speech::vectorizer::MFCCVectorizer;

#include "speech/HMMLexicon.h"

using speech::HMMLexicon;

#include "speech/initializer/RandomInitializer.h"
#include "speech/initializer/KMeansInitializer.h"

using speech::initializer::AbstractGaussianInitializer;
using speech::initializer::RandomInitializer;
using speech::initializer::KMeansInitializer;

#include "speech/helpers.h"
#include "CVDataSet.hpp"

#include <jsoncpp/json/json.h>

int main(int argc, char **argv) {
    // Check if there is a configuration file path passed as a first
    // parameter of the application and serve the error if there is not
    if (argc != 2) {
        std::cerr << "This application should be run with a path "
                "to JSON file containing the configuration "
                "of the model" << std::endl;
        return 1;
    }

    // Tries to open provided file containing the configuration of the model
    std::string inputFilename(argv[1]);
    std::ifstream inputFile;
    inputFile.open(inputFilename, std::ifstream::in);
    if (false == inputFile.good()) {
        std::cerr << "Given JSON file does not exist" << std::endl;
        return 2;
    }

    // Parse provided JSON file
    Json::CharReaderBuilder charReaderBuilder;
    Json::Value root;
    std::string errors;
    bool parsedSuccessfully = Json::parseFromStream(charReaderBuilder, inputFile, &root, &errors);
    if (!parsedSuccessfully) {
        std::cerr << "Parsing given JSON file was not possible due to the following errors:" << std::endl;
        std::cerr << errors;
        return 3;
    }

    const unsigned int CROSS_VALIDATION_K = 27; // number of different voices
    const unsigned int TOP_PREDICTIONS = 10;

    const int SAMPLE_LENGTH = root["sample_length"].asInt(); // in milliseconds
    const int SAMPLE_OFFSET = root["sample_offset"].asInt(); // in milliseconds
    const int MFCC_BINS = root["mfcc_bins"].asInt(); // number of Mel filters in a bank
    const int MFCC_CEPSTRAL_COEFFICIENTS = root["mfcc_cepstral_coefficients"].asInt(); // number of cepstral coefficients used for analysis
    const double MFCC_MIN_FREQUENCY = root["mfcc_min_frequency"].asDouble(); // in Herzs
    const double MFCC_MAX_FREQUENCY = root["mfcc_max_frequency"].asDouble(); // in Herzs
    const int LEXICON_DIMENSIONALITY = 3 * (MFCC_CEPSTRAL_COEFFICIENTS + 1);
    const int LEXICON_GAUSSIANS = root["lexicon_gaussians"].asInt();
    const unsigned int MAX_ITERATIONS = root["max_iterations"].asInt();

    MFCCVectorizer<short int> *mfccVectorizer = new MFCCVectorizer<short int>(MFCC_BINS, MFCC_CEPSTRAL_COEFFICIENTS,
                                                                              MFCC_MIN_FREQUENCY, MFCC_MAX_FREQUENCY,
                                                                              SAMPLE_LENGTH, SAMPLE_OFFSET);

    int allUtterancesCount = 0;
    int predictedUtterancesCount = 0;
    std::map<std::string, int> wordPredictions;
    std::map<std::string, int> wordInTopPredictions;
    std::vector<CVDataSet> datasets = divideForCrossValidation(root, CROSS_VALIDATION_K);
    for (auto it = datasets.begin(); it != datasets.end(); it++) {
        CVDataSet &dataset = *it;
        for (auto trainIt = dataset.trainingFiles.begin(); trainIt != dataset.trainingFiles.end(); trainIt++) {
            WaveFileDataSource<short int> dataSource(trainIt->second, SAMPLE_LENGTH);
            dataSource.init();
            HMMLexicon::Observation utterance = mfccVectorizer->vectorize(dataSource);
            dataset.hmmLexicon->addUtterance(utterance, trainIt->first);
            if (wordPredictions.find(trainIt->first) == wordPredictions.end()) {
                wordPredictions[trainIt->first] = 0;
                wordInTopPredictions[trainIt->first] = 0;
            }
        }

        // Fit the model
        dataset.hmmLexicon->fit();

        // Check the predictions of test vectors
        for (auto testIt = dataset.testFiles.begin(); testIt != dataset.testFiles.end(); testIt++) {
            WaveFileDataSource<short int> dataSource(testIt->second, SAMPLE_LENGTH);
            dataSource.init();
            HMMLexicon::Observation utterance = mfccVectorizer->vectorize(dataSource);
            std::string prediction = dataset.hmmLexicon->predict(utterance);
            std::map<std::string, double> bestPredictions = dataset.hmmLexicon->getBestPredictions(utterance,
                                                                                                   TOP_PREDICTIONS);

            // Check if the prediction was correct
            allUtterancesCount++;
            if (prediction.compare(testIt->first) == 0) {
                predictedUtterancesCount++;
                wordPredictions[testIt->first]++;
            }

            // Check if the prediction was close to correct one
            for (auto it = bestPredictions.begin(); it != bestPredictions.end(); it++) {
                if (it->first.compare(testIt->first) == 0) {
                    wordInTopPredictions[testIt->first]++;
                }
            }
        }
    }

    inputFile.close();

    std::cout << "All utterances: " << allUtterancesCount << std::endl;
    std::cout << "Predicted utterances: " << predictedUtterancesCount << std::endl;
    for (auto it = wordPredictions.begin(); it != wordPredictions.end(); it++) {
        std::cout << "Word '" << it->first << "' predictions: " << it->second << std::endl;
    }
    for (auto it = wordInTopPredictions.begin(); it != wordInTopPredictions.end(); it++) {
        std::cout << "Word '" << it->first << "' in top " << TOP_PREDICTIONS << " predictions: " << it->second << std::endl;
    }

    delete mfccVectorizer;

    return 0;
}

