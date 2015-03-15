#include <iostream>
#include <list>

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

using speech::LanguageModel;

//
// This is an entry point of the application. It allows user to select the data source used
// to get an input data, choose the methods of signal transformation into frequency domain
// and a spelling transcription.
//
// @todo create a logic
//
int main(int argc, char **argv) {
    const int singleDataVectorDimension = 10;   // dimension of the vector describing single sample
    const int numberOfPhonems = 4;              // number of clusters used by the clustering method
    const int numberOfLetters = 4;              // number of symbols used in the spelling transcription

    std::vector<double *> vectors;              // list of vectors produced by all data sources
    std::vector<int> labels;                    // list of labels can be empty if used clustering method
    // uses unsupervised learning, i.e. KMeans

    std::vector<const char *> dataSources;
    dataSources.push_back("file_1.wav");        // @todo: list should be more dynamic, but it's not necessary now

    std::vector<std::string> transcriptions;
    transcriptions.push_back("text one");       // @todo: as above

    for (auto it = dataSources.begin(); it != dataSources.end(); it++) {
        WaveFileDataSource<signed char> *dataSourcePtr = new WaveFileDataSource<signed char>(*it);

        // @todo: add each vector produced by the data source into vectors
        // vectors.push_back(vector);

        delete dataSourcePtr;
    }

    IClusteringMethod *clusteringMethodPtr = new KMeans(numberOfPhonems, singleDataVectorDimension);
    ISpellingTranscription *spellingMethodPtr = new HMM(numberOfPhonems, numberOfLetters);

    try {
        clusteringMethodPtr->fit(vectors, labels);

        int dataSourcesSize = dataSources.size();
        for (int i = 0; i < dataSourcesSize; i++) {
            const char *fileName = dataSources.at(i);
            std::string &transcription = transcriptions.at(i);

            WaveFileDataSource<signed char> *dataSourcePtr = new WaveFileDataSource<signed char>(fileName);

            std::vector<int> predictedLabels;
            // @todo: iterate through all vectors from current data source, classify them using cllusteringMethod and add them into predictedLabels

            spellingMethodPtr->fit(predictedLabels, transcription);

            delete dataSourcePtr;
        }

        LanguageModel languageModel(clusteringMethodPtr, spellingMethodPtr);
        // @todo store a model into a file to be loaded and used for classification purposes

        // the model is fitted and can be used to transcript another data sources (validation)
        // @todo: add a validation
    } catch (speech::clustering::exception::TooLessVectorsException &ex) {
        std::cerr << "You need to provide at least " << numberOfPhonems
                << " vectors to perform the clustering" << std::endl;
    }

    delete clusteringMethodPtr;
    delete spellingMethodPtr;

    return 0;
}