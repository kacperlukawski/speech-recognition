#include <iostream>
#include <memory>
#include <vector>
#include <string>

using std::shared_ptr;
using std::vector;
using std::string;

#include "speech/raw_data/WaveFileDataSource.h"

using speech::raw_data::DataSource;
using speech::raw_data::WaveFileDataSource;

#include "speech/LanguageModel.h"

using speech::LanguageModel;

#include "speech/vectorizer/ThirdsPowerVectorizer.h"
#include "speech/vectorizer/HarmonicsVectorizer.h"
#include "speech/vectorizer/MFCCVectorizer.h"

using speech::vectorizer::IVectorizer;
using speech::vectorizer::ThirdsPowerVectorizer;
using speech::vectorizer::HarmonicsVectorizer;
using speech::vectorizer::MFCCVectorizer;

#include "speech/clustering/KMeans.h"
#include "speech/clustering/GaussianMixtureModel.h"

using speech::clustering::IClusteringMethod;
using speech::clustering::KMeans;
using speech::clustering::GaussianMixtureModel;

#include "speech/spelling/HMM.h"

using speech::spelling::ISpellingTranscription;
using speech::spelling::HMM;

/**
 * Spectrum is a squared magnitude of the FFT.
 * @see http://en.wikipedia.org/wiki/Spectrogram
 */
int main(int argc, char **argv) {
    const int phonemesNumber = 4;   // number of phonemes generated by the clustering method
    const int sampleLength = 20;    // length in milliseconds

    // TODO: distance function needs to be changed! dimensions should be as independent as possible

    shared_ptr<IVectorizer<short>> vectorizer =
//            shared_ptr<IVectorizer<short>>(new ThirdsPowerVectorizer<short>());
//            shared_ptr<IVectorizer<short>>(new HarmonicsVectorizer<short>(25));
            shared_ptr<IVectorizer<short>>(new MFCCVectorizer<short>(26, 12));
    shared_ptr<IClusteringMethod> clusteringMethod =
//            shared_ptr<IClusteringMethod>(new KMeans(phonemesNumber, vectorizer->getVectorSize()));
            shared_ptr<IClusteringMethod>(new GaussianMixtureModel());
    shared_ptr<ISpellingTranscription> spellingTranscription =
            shared_ptr<ISpellingTranscription>(new HMM(4, 10));
    LanguageModel<short> languageModel(vectorizer,
                                       clusteringMethod,
                                       spellingTranscription);

    vector<DataSource<short>> dataSources;
    dataSources.push_back(WaveFileDataSource<short>("/home/kacper/voice/dziadzio.wav", sampleLength));
    dataSources.push_back(WaveFileDataSource<short>("/home/kacper/voice/dziadzio_1.wav", sampleLength));
    dataSources.push_back(WaveFileDataSource<short>("/home/kacper/voice/dziadzio_2.wav", sampleLength));

    vector<string> transcriptions;
    transcriptions.push_back("dziadzio");
    transcriptions.push_back("dziadzio");
    transcriptions.push_back("dziadzio");

    languageModel.fit(dataSources, transcriptions);

    // TODO: check the vectorizer, because it returns different values after using it for a set of vectors

    std::cout << std::endl << "Predictions:" << std::endl;
    languageModel.predict(dataSources[0]);
    languageModel.predict(dataSources[1]);
    languageModel.predict(dataSources[2]);

    return 0;
}