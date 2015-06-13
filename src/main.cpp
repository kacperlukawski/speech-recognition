#include <iostream>
#include <memory>
#include <list>
#include <streambuf>

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

#include "speech/clustering/exception/TooLessVectorsException.h"
#include "speech/LanguageModel.h"
#include "speech/transform/IFrequencyTransform.h"
#include "speech/transform/FastFourierTransform.h"

using speech::LanguageModel;
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

//
// Gets the vector containing all letters occuring in given collection
// of words. The result does not contain duplicates an blank characters.
//
std::vector<char> getUniqueLetters(std::vector<std::string> &transcriptions) {
    std::vector<char> letters;
    for (auto it = transcriptions.begin(); it != transcriptions.end(); it++) {
        int length = (*it).size();
        for (int i = 0; i < length; i++) {
            char letter = (*it)[i];

            // blank characters are not included
            if (letter == ' ' || letter == '\t' || letter == '\n' || letter == '\r') {
                continue;
            }

            auto pos = std::find(letters.begin(), letters.end(), letter);
            if (pos != letters.end()) {
                continue;
            }

//            std::cout << letter << " ";

            letters.push_back(letter);
        }
    }

    return letters;
}

void str_replace(string &s, const string &search, const string &replace) {
    for (size_t pos = 0; ; pos += replace.length()) {
        pos = s.find(search, pos);
        if (pos == string::npos) break;

        s.erase(pos, search.length());
        s.insert(pos, replace);
    }
}

/**
 * Loads whole content of the given file and returns it as a string
 *
 * @return file content
 */
std::string getFileContent(const std::string &filepath) {
    std::ifstream file(filepath.c_str());
    std::string content = std::string(std::istreambuf_iterator<char>(file),
                                      std::istreambuf_iterator<char>());
    file.close();

    return content;
}

/**
 * This is an entry point of the application. It allows user to select the data source used
 * to get an input data, choose the methods of signal transformation into frequency domain
 * and a spelling transcription.
 *
 * @todo create a logic
 */
int main(int argc, char **argv) {
    const int singleSampleLength = 20;           // length of the DataSample in milliseconds
    const int numberOfPhonems = 91;              // number of clusters used by the clustering method

    // @todo: list should be more dynamic, but it's not necessary now
    std::vector<const char *> sourceFiles;
    sourceFiles.push_back("/home/kacper/Projects/speech-recognition/dataset/records/record_0.wav");
    sourceFiles.push_back("/home/kacper/Projects/speech-recognition/dataset/records/record_1.wav");
    sourceFiles.push_back("/home/kacper/Projects/speech-recognition/dataset/records/record_2.wav");
    sourceFiles.push_back("/home/kacper/Projects/speech-recognition/dataset/records/record_3.wav");
    sourceFiles.push_back("/home/kacper/Projects/speech-recognition/dataset/records/record_3_a.wav");
//    sourceFiles.push_back("/home/kacper/Projects/speech-recognition/dataset/records/record_4.wav");
//    sourceFiles.push_back("/home/kacper/Projects/speech-recognition/dataset/records/record_5.wav");
//    sourceFiles.push_back("/home/kacper/voice/samogloski.wav");

    std::string record0Content = getFileContent(
            "/home/kacper/Projects/speech-recognition/dataset/variants/words_0.txt");
    str_replace(record0Content, ",", "");

    std::string record1Content = getFileContent(
            "/home/kacper/Projects/speech-recognition/dataset/variants/words_1.txt");
    str_replace(record1Content, ",", "");

    std::string record2Content = getFileContent(
            "/home/kacper/Projects/speech-recognition/dataset/variants/words_2.txt");
    str_replace(record2Content, ",", "");

    std::string record3Content = getFileContent(
            "/home/kacper/Projects/speech-recognition/dataset/variants/words_3.txt");
    str_replace(record3Content, ",", "");

    std::string record4Content = getFileContent(
            "/home/kacper/Projects/speech-recognition/dataset/variants/words_4.txt");
    str_replace(record4Content, ",", "");

    std::string record5Content = getFileContent(
            "/home/kacper/Projects/speech-recognition/dataset/variants/words_5.txt");
    str_replace(record5Content, ",", "");

    std::vector<std::string> transcriptions;
    transcriptions.push_back(record0Content);
    transcriptions.push_back(record1Content);
    transcriptions.push_back(record2Content);
    transcriptions.push_back(record3Content);
    transcriptions.push_back(record3Content);
//    transcriptions.push_back(record4Content);
//    transcriptions.push_back(record5Content);

    std::vector<DataSource<short>> dataSources;
    for (auto it = sourceFiles.begin(); it != sourceFiles.end(); it++) {
        dataSources.push_back(WaveFileDataSource<short>(*it, singleSampleLength));
    }

    std::vector<char> letters = getUniqueLetters(transcriptions);

    shared_ptr<IVectorizer<short>> vectorizerPtr = shared_ptr<IVectorizer<short>>(
//            new FormantVectorizer<short>());
//            new ThirdsPowerVectorizer<short>());
            new MFCCVectorizer<short>(26, 12));
    shared_ptr<IClusteringMethod> clusteringMethodPtr = shared_ptr<IClusteringMethod>(
//            new KMeans(numberOfPhonems, vectorizerPtr->getVectorSize()));
            new GaussianMixtureModel(numberOfPhonems, vectorizerPtr->getVectorSize()));
    shared_ptr<ISpellingTranscription> spellingMethodPtr = shared_ptr<ISpellingTranscription>(
            new HMM(numberOfPhonems, letters));
    LanguageModel<short> languageModel(vectorizerPtr, clusteringMethodPtr, spellingMethodPtr);

//    std::ifstream input("/home/kacper/voice/model_Wed May 27 22:30:27 2015.lm");
//    LanguageModel<short> languageModel(input);

    try {
        // fit the model using data taken from source files
        languageModel.fit(dataSources, transcriptions);

        // add a new file to be checked
        dataSources.push_back(
                WaveFileDataSource<short>("/home/kacper/voice/samogloski_100-110_100-110_100-110_100-110.wav",
                                          singleSampleLength));
        dataSources.push_back(
                WaveFileDataSource<short>("/home/kacper/voice/samogloski_1_2_3_4_5_6_7_8_9_10_11_12-25.wav",
                                          singleSampleLength));
        dataSources.push_back(
                WaveFileDataSource<short>("/home/kacper/voice/dziadzio.wav",
                                          singleSampleLength));
        dataSources.push_back(
                WaveFileDataSource<short>("/home/kacper/voice/sala.wav",
                                          singleSampleLength));

        // test files one by one and try to predict the transcription
        for (auto it = dataSources.begin(); it != dataSources.end(); it++) {
            std::cout << "predicted: " << languageModel.predict(*it) << std::endl;
        }

//        char *modelFilename = new char[100];
//        sprintf(modelFilename, "/home/kacper/voice/model_%s.lm", __TIMESTAMP__);
//        std::ofstream output(modelFilename);
//        output << languageModel;
//        output.close();
//        delete[] modelFilename;

        // the model is fitted and can be used to transcript another data sources (validation)
        // @todo: add a validation
    } catch (speech::clustering::exception::TooLessVectorsException &ex) {
        std::cerr << "You need to provide at least " << numberOfPhonems
                  << " vectors to perform the clustering" << std::endl;
    }

    return 0;
}