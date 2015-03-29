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
#include "speech/transform/IFrequencyTransform.h"
#include "speech/transform/FastFourierTransform.h"

using speech::LanguageModel;
using namespace speech::transform;

#include "speech/vectorizer/IVectorizer.h"
#include "speech/vectorizer/MaxFrequencyVectorizer.h"

using speech::vectorizer::IVectorizer;
using speech::vectorizer::MaxFrequencyVectorizer;

//
// This is an entry point of the application. It allows user to select the data source used
// to get an input data, choose the methods of signal transformation into frequency domain
// and a spelling transcription.
//
// @todo create a logic
//
int main(int argc, char **argv) {
    const int singleDataVectorDimension = 40;   // dimension of the vector describing single sample
    const int numberOfPhonems = 6;              // number of clusters used by the clustering method
//    const int numberOfLetters = 10;              // number of symbols used in the spelling transcription

    std::vector<std::shared_ptr<double>> tmp;
    std::vector<double *> vectors;              // list of vectors produced by all data sources
    std::vector<int> labels;                    // list of labels can be empty if used clustering method
    // uses unsupervised learning, i.e. KMeans

    std::vector<const char *> dataSources;
    dataSources.push_back("/home/kacper/voice/linda.wav");        // @todo: list should be more dynamic, but it's not necessary now

    std::vector<std::string> transcriptions;
    transcriptions.push_back("wypierdalac");

    std::vector<char> letters;
    for (auto it = transcriptions.begin(); it != transcriptions.end(); it++) {
        int length = (*it).size();
        for (int i = 0; i < length; i++) {
            char letter = (*it)[i];
            auto pos = std::find(letters.begin(), letters.end(), letter);
            if (pos != letters.end()) {
                continue;
            }

            std::cout << letter << " ";

            letters.push_back(letter);
        }
    }

    std::cout << std::endl;

    IFrequencyTransform<short> *fft = new FastFourierTransform<short>();
    IVectorizer<short> *vectorizer = new MaxFrequencyVectorizer<short>(singleDataVectorDimension / 4);

    for (auto it = dataSources.begin(); it != dataSources.end(); it++) {
        WaveFileDataSource<short> *dataSourcePtr = new WaveFileDataSource<short>(*it);

        auto begin = dataSourcePtr->getSamplesIteratorBegin();
        auto end = dataSourcePtr->getSamplesIteratorEnd();

        for (auto innerIt = begin; innerIt != end; innerIt++) {
            FrequencySample<short> frequencySample = fft->transform(*innerIt);
            std::vector<double> vector = vectorizer->vectorize(frequencySample);

//            std::cout << "vector(" << vector.size() << "): ";
//            for (auto it = vector.begin(); it != vector.end(); it++) {
//                std::cout << *it << " ";
//            }
//            std::cout << "\n";

            double *rawDataVector = new double[singleDataVectorDimension];
            for (int i = 0; i < singleDataVectorDimension; i++) {
                double value = 0.0;
                if (i < vector.size()) {
                    value = vector.at(i);
                }

                rawDataVector[i] = value;
            }

            vectors.push_back(rawDataVector);

//            std::shared_ptr<double> a = frequencySample.getAmplitude();
//            tmp.push_back(a);
//            vectors.push_back(a.get());
        }

        delete dataSourcePtr;
    }


//    for (auto it = vectors.begin(); it != vectors.end(); it++) {
//        for (int pos = 0; pos < 100; pos++) {
//            std::cout << (*it)[pos] << " ";
//        }
//        std::cout << std::endl;
//    }

    IClusteringMethod *clusteringMethodPtr = new KMeans(numberOfPhonems, singleDataVectorDimension);
    ISpellingTranscription *spellingMethodPtr = new HMM(numberOfPhonems, letters);

    try {
        clusteringMethodPtr->fit(vectors, labels);

        int dataSourcesSize = dataSources.size();
        for (int i = 0; i < dataSourcesSize; i++) {
            const char *fileName = dataSources.at(i);
            std::string &transcription = transcriptions.at(i);

            WaveFileDataSource<short> *dataSourcePtr = new WaveFileDataSource<short>(fileName);

            auto begin = dataSourcePtr->getSamplesIteratorBegin();
            auto end = dataSourcePtr->getSamplesIteratorEnd();

            std::vector<int> predictedLabels;
            for (auto innerIt = begin; innerIt != end; innerIt++) {
                FrequencySample<short> frequencySample = fft->transform(*innerIt);
                std::vector<double> vector = vectorizer->vectorize(frequencySample);

//            std::cout << "vector(" << vector.size() << "): ";
//            for (auto it = vector.begin(); it != vector.end(); it++) {
//                std::cout << *it << " ";
//            }
//            std::cout << "\n";

                double *rawDataVector = new double[singleDataVectorDimension];
                for (int i = 0; i < singleDataVectorDimension; i++) {
                    double value = 0.0;
                    if (i < vector.size()) {
                        value = vector.at(i);
                    }

                    rawDataVector[i] = value;
                }

                int label = clusteringMethodPtr->predict(rawDataVector);
                predictedLabels.push_back(label);

                std::cout << label;
            }

            std::cout << std::endl;

            spellingMethodPtr->fit(predictedLabels, transcription);

            delete dataSourcePtr;
        }

        LanguageModel languageModel(clusteringMethodPtr, spellingMethodPtr);

        for (auto it = dataSources.begin(); it != dataSources.end(); it++) {
            WaveFileDataSource<short> *dataSourcePtr = new WaveFileDataSource<short>(*it);

            auto begin = dataSourcePtr->getSamplesIteratorBegin();
            auto end = dataSourcePtr->getSamplesIteratorEnd();

            std::vector<int> predictedLabels;
            for (auto innerIt = begin; innerIt != end; innerIt++) {
                FrequencySample<short> frequencySample = fft->transform(*innerIt);
                std::vector<double> vector = vectorizer->vectorize(frequencySample);

                double *rawDataVector = new double[singleDataVectorDimension];
                for (int i = 0; i < singleDataVectorDimension; i++) {
                    double value = 0.0;
                    if (i < vector.size()) {
                        value = vector.at(i);
                    }

                    rawDataVector[i] = value;
                }

                int label = clusteringMethodPtr->predict(rawDataVector);
                predictedLabels.push_back(label);
            }

            std::cout << "predicted: " << spellingMethodPtr->predict(predictedLabels) << std::endl;

            delete dataSourcePtr;
        }

        //std::cout << languageModel;
        // @todo store a model into a file to be loaded and used for classification purposes

        // the model is fitted and can be used to transcript another data sources (validation)
        // @todo: add a validation
    } catch (speech::clustering::exception::TooLessVectorsException &ex) {
        std::cerr << "You need to provide at least " << numberOfPhonems
                << " vectors to perform the clustering" << std::endl;
    }

    delete fft;
    delete clusteringMethodPtr;
    delete spellingMethodPtr;

    return 0;
}