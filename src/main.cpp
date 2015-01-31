#include <iostream>
#include <list>

#include "speech/raw_data/WaveFileDataSource.h"

using speech::raw_data::WaveFileDataSource;

#include "speech/raw_data/DataSample.h"

#include "speech/transform/IFrequencyTransform.h"

#include "speech/clustering/KMeans.h"

using speech::clustering::IClusteringMethod;
using speech::clustering::KMeans;

#include "speech/spelling/ISpellingTranscription.h"

using speech::spelling::ISpellingTranscription;

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
    WaveFileDataSource<signed char> *dataSourcePtr = new WaveFileDataSource<signed char>("lorem ipsum");
    IClusteringMethod *clusteringMethodPtr = new KMeans(2, 2);
    ISpellingTranscription *spellingMethodPtr = nullptr;

    LanguageModel *languageModel = new LanguageModel(clusteringMethodPtr, spellingMethodPtr);
    std::cout << *languageModel;

    // for each sample from the source file transform it into frequency domain
    // and try to cluster it to one of the groups - number of the groups should
    // be defined as a number of phonems which we're trying to recognize
    // then use sequencies of phonems to receive the most probable spelling
    // of the words

    return 0;
}