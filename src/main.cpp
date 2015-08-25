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

#include "speech/helpers.h"

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

    // Get the list of the words' transcriptions and assigned files
    // Load all the files and build a model for each utterance
    Json::CharReaderBuilder charReaderBuilder;
    std::ifstream inputFile("input.json");
    Json::Value root;
    std::string errors;
    Json::parseFromStream(charReaderBuilder, inputFile, &root, &errors);

    const int SAMPLE_LENGTH = 20; // in milliseconds
    const int SAMPLE_OFFSET = 10; // in milliseconds
    const int MFCC_BINS = 26; // number of Mel filters in a bank
    const int MFCC_CEPSTRAL_COEFFICIENTS = 12; // number of cepstral coefficients used for analysis
    const double MFCC_MIN_FREQUENCY = 64.0; // in Herzs
    const double MFCC_MAX_FREQUENCY = 16000.0; // in Herzs
    const int LEXICON_DIMENSIONALITY = 3 * (MFCC_CEPSTRAL_COEFFICIENTS + 1);
    const int LEXICON_GAUSSIANS = 16;

    HMMLexicon lexicon(LEXICON_DIMENSIONALITY, LEXICON_GAUSSIANS);
    MFCCVectorizer<short int> *mfccVectorizer = new MFCCVectorizer<short int>(MFCC_BINS, MFCC_CEPSTRAL_COEFFICIENTS,
                                                                              MFCC_MIN_FREQUENCY, MFCC_MAX_FREQUENCY,
                                                                              SAMPLE_LENGTH, SAMPLE_OFFSET);

    int trainFilesNumber = 4;
    int testFilesNumber = 5;
    std::string filenames[] = {
            "/home/kacper/Projects/speech-recognition/dataset/splitted_records/record1/sala.wav",
            "/home/kacper/Projects/speech-recognition/dataset/splitted_records/record2/sala.wav",
//            "/home/kacper/Projects/speech-recognition/dataset/splitted_records/record3/sala.wav",
            "/home/kacper/Projects/speech-recognition/dataset/splitted_records/record4/sala.wav",
            "/home/kacper/Projects/speech-recognition/dataset/splitted_records/record5/sala.wav",
            "/home/kacper/Projects/speech-recognition/dataset/splitted_records/record6/sala.wav"
    };
    std::string transcriptions[] = {
            "s|a|l|a",
            "s|a|l|a",
            "s|a|l|a",
            "s|a|l|a",
            "s|a|l|a",
            "s|a|l|a"
    };

    // do the following process for each file from the collection
    for (int i = 0; i < trainFilesNumber; ++i) {
        WaveFileDataSource<short int> dataSource(filenames[i], SAMPLE_LENGTH);
        HMMLexicon::Observation utterance = mfccVectorizer->vectorize(dataSource);
        lexicon.addUtterance(utterance, transcriptions[i], "|");
    }

    lexicon.fit();

    // get the same set once again and test what the model guesses about each utterance
    for (int i = 0; i < testFilesNumber; ++i) {
        WaveFileDataSource<short int> dataSource(filenames[i], SAMPLE_LENGTH);
        HMMLexicon::Observation utterance = mfccVectorizer->vectorize(dataSource);
        lexicon.predict(utterance);
    }

    delete mfccVectorizer;

    return 0;
}