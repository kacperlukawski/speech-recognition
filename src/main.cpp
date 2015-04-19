#include <iostream>
#include <memory>
#include <list>

using std::shared_ptr;

#include "speech/raw_data/WaveFileDataSource.h"

using speech::raw_data::WaveFileDataSource;

#include "speech/raw_data/DataSample.h"

#include "speech/transform/IFrequencyTransform.h"

#include "speech/clustering/KMeans.h"

using speech::clustering::IClusteringMethod;
using speech::clustering::KMeans;

#include "speech/spelling/HMM.h"

using speech::spelling::ISpellingTranscription;
using speech::spelling::HMM;

#include <vector>
#include <clustering/exception/TooLessVectorsException.h>

#include "speech/LanguageModel.h"
#include "speech/transform/IFrequencyTransform.h"
#include "speech/transform/FastFourierTransform.h"

using speech::LanguageModel;
using namespace speech::transform;

#include "speech/vectorizer/IVectorizer.h"
#include "speech/vectorizer/MaxFrequencyVectorizer.h"

using speech::vectorizer::IVectorizer;
using speech::vectorizer::MaxFrequencyVectorizer;

//
// Overloads the << operator for std::valarray to have a simple method
// to display current values of vectors.
//
std::ostream &operator<<(std::ostream &out, std::valarray<double> vector) {
    for (int i = 0; i < vector.size(); i++) {
        out << vector[i] << " ";
    }

    return out;
}

//
// Gets the vector containing all letters occuring in given collection
// of words. The result does not contain duplicates.
//
std::vector<char> getUniqueLetters(std::vector<std::string> &transcriptions) {
    std::vector<char> letters;
    for (auto it = transcriptions.begin(); it != transcriptions.end(); it++) {
        int length = (*it).size();
        for (int i = 0; i < length; i++) {
            char letter = (*it)[i];
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

//
// This is an entry point of the application. It allows user to select the data source used
// to get an input data, choose the methods of signal transformation into frequency domain
// and a spelling transcription.
//
// @todo create a logic
//
int main(int argc, char **argv) {
    const int singleDataVectorDimension = 40;   // dimension of the vector describing single sample
    const int numberOfPhonems = 4;              // number of clusters used by the clustering method

    std::vector<const char *> sourceFiles;
    sourceFiles.push_back(
            "/home/kacper/voice/samogloski.wav");        // @todo: list should be more dynamic, but it's not necessary now
//    sourceFiles.push_back("/home/kacper/voice/linda.wav");

    std::vector<std::string> transcriptions;
    transcriptions.push_back("aaaaaaaaaeeeeeeeeeiiiiiiiiiiiii");
//    transcriptions.push_back("wypierdalac");

    std::vector<DataSource<short>> dataSources;
    for (auto it = sourceFiles.begin(); it != sourceFiles.end(); it++) {
        dataSources.push_back(WaveFileDataSource<short>(*it));
    }

//    double init[] = {1.0, 2.0, 3.0};
//    std::valarray<double> test(init, 3);
//    std::cout << /*"test = " << */test << std::endl;
    std::vector<char> letters = getUniqueLetters(transcriptions);

    shared_ptr<IVectorizer<short>> vectorizerPtr = shared_ptr<IVectorizer<short>>(
            new MaxFrequencyVectorizer<short>(singleDataVectorDimension));
    shared_ptr<IClusteringMethod> clusteringMethodPtr = shared_ptr<IClusteringMethod>(
            new KMeans(numberOfPhonems, singleDataVectorDimension));
    shared_ptr<ISpellingTranscription> spellingMethodPtr = shared_ptr<ISpellingTranscription>(
            new HMM(numberOfPhonems, letters));
    LanguageModel<short> languageModel(vectorizerPtr, clusteringMethodPtr, spellingMethodPtr);

    try {
        // fit the model using data taken from source files
        languageModel.fit(dataSources, transcriptions);

        // test files one by one and try to predict the transcription
        for (auto it = dataSources.begin(); it != dataSources.end(); it++) {
            std::cout << "predicted: " << languageModel.predict(*it) << std::endl;
        }

        //std::cout << languageModel;
        // @todo store a model into a file to be loaded and used for classification purposes

        // the model is fitted and can be used to transcript another data sources (validation)
        // @todo: add a validation
    } catch (speech::clustering::exception::TooLessVectorsException &ex) {
        std::cerr << "You need to provide at least " << numberOfPhonems
                << " vectors to perform the clustering" << std::endl;
    }

    return 0;
}