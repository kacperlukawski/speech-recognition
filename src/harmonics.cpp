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

#include "speech/HMMLexicon.h"

using speech::HMMLexicon;

/**
 * Spectrum is a squared magnitude of the FFT.
 * @see http://en.wikipedia.org/wiki/Spectrogram
 */
int main(int argc, char **argv) {
    std::cout.precision(15);

    const int WINDOW_SIZE = 20;
    const int WINDOW_OFFSET = 10;
    const int MFCC_BINS = 26;
    const int MFCC_COEFFS = 13;
    const double MFCC_MIN_FREQUENCY = 0.0;
    const double MFCC_MAX_FREQUENCY = 16000.0;

    WaveFileDataSource<short int> dataSource("/home/kacper/Projects/mfcc-features-check/dzianina.wav", WINDOW_SIZE);

    FastFourierTransform<short int> fft;
    MFCCVectorizer<short int> mfcc(MFCC_BINS, MFCC_COEFFS, MFCC_MIN_FREQUENCY, MFCC_MAX_FREQUENCY, WINDOW_SIZE, WINDOW_OFFSET);

    int sampleCount = 0;
    for (auto it = dataSource.getOffsetIteratorBegin(WINDOW_SIZE, WINDOW_OFFSET);
         it != dataSource.getOffsetIteratorEnd(); ++it) {
        const DataSample<short int> &dataSample = *it;
        sampleCount++;
    }

    std::cout << "sampleCount: " << sampleCount << std::endl;

    vector<valarray<double>> vectors = mfcc.vectorize(dataSource);
    std::cout << "vectors.size(): " << vectors.size() << std::endl;

    for (int i = 0; i < vectors.size(); i++)
        std::cout << vectors[i] << std::endl;

    return 0;

//    const int dimensionality = 3; // dimenstionality of a single data vector
//    const int gaussians = 16; // number of Gaussians in a single state of HMM
//
//    valarray<double> v1(0.0, dimensionality);
//    v1[0] = 1.0;
//    v1[1] = 3.5;
//    v1[2] = -3.5;
//
//    valarray<double> v2(0.0, dimensionality);
//    v2[0] = 7.0;
//    v2[1] = 1.5;
//    v2[2] = -50;
//
//    valarray<double> v3(0.0, dimensionality);
//    v3[0] = 17.0;
//    v3[1] = -1.5;
//    v3[2] = 1.354;
//
//    HMMLexicon::Observation lorem;
//    lorem.push_back(v1);
////    lorem.push_back(v1);
////    lorem.push_back(v1);
////    lorem.push_back(v1);
////    lorem.push_back(-v1);
////    lorem.push_back(-v1);
////    lorem.push_back(-v1);
//    lorem.push_back(-v2);
////    lorem.push_back(0.0 * v1); // silence at the end
//    lorem.push_back(2.7 * v2 - 0.4 * v1);
//
//    HMMLexicon::Observation lorem_2;
//    lorem_2.push_back(v1 * 2.0);
////    lorem_2.push_back(v1 * 2.0);
////    lorem_2.push_back(v1 * 2.0);
////    lorem_2.push_back(v1 * - 2.0);
////    lorem_2.push_back(v1 * - 2.0);
//    lorem_2.push_back(v1 * - 2.0);
//
//    HMMLexicon::Observation ipsum;
//    ipsum.push_back(v3);
//    ipsum.push_back(v3);
//    ipsum.push_back(v3);
//    ipsum.push_back(1.75 * v2);
//    ipsum.push_back(1.75 * v2);
//    ipsum.push_back(1.75 * v2);
//    lorem_2.push_back(v1 * 10.0);
//
//    HMMLexicon lexicon(dimensionality, gaussians);
//    lexicon.addUtterance(lorem, "lo|rem");
//    lexicon.addUtterance(lorem_2, "lo|rem");
//    lexicon.addUtterance(ipsum, "ip|sum");
//    lexicon.fit();
//
//    std::cout << "lexicon size: " << lexicon.size() << std::endl << std::endl;
//    std::cout << "lexicon.predict(lorem) = " << lexicon.predict(lorem) << std::endl;
//    std::cout << "lexicon.predict(lorem_2) = " << lexicon.predict(lorem_2) << std::endl;
//    std::cout << "lexicon.predict(ipsum) = " << lexicon.predict(ipsum) << std::endl;
//
////    int gaussians = 3;
////    int dimensionality = 1;
////    HMMLexicon::GMMLikelihoodFunction f(gaussians, dimensionality);
//////    f.setMean(0, 0, 2.0);
////    std::cout << "f(0.0) = " << f(valarray<double>(0.0, dimensionality)) << "\n";
////    std::cout << "f(1.0) = " << f(valarray<double>(1.0, dimensionality)) << "\n";
////    std::cout << "f(-1.0) = " << f(valarray<double>(-1.0, dimensionality)) << "\n";
////    std::cout << "f(10.0) = " << f(valarray<double>(10.0, dimensionality)) << "\n";
//
//    return 0;
//
//    const int phonemesNumber = 5;   // number of phonemes generated by the clustering method
//    const int sampleLength = 20;    // length in milliseconds
//
//    // TODO: distance function needs to be changed! dimensions should be as independent as possible
//
//    shared_ptr<IVectorizer<short>> vectorizer =
////            shared_ptr<IVectorizer<short>>(new ThirdsPowerVectorizer<short>());
////            shared_ptr<IVectorizer<short>>(new HarmonicsVectorizer<short>(25));
//            shared_ptr<IVectorizer<short>>(new MFCCVectorizer<short>(23, 13));
//    shared_ptr<IClusteringMethod> clusteringMethod =
////            shared_ptr<IClusteringMethod>(new KMeans(phonemesNumber, vectorizer->getVectorSize()));
//            shared_ptr<IClusteringMethod>(new GaussianMixtureModel(phonemesNumber, vectorizer->getVectorSize()));
//    shared_ptr<ISpellingTranscription> spellingTranscription =
//            shared_ptr<ISpellingTranscription>(new HMM(4, 10));
//    LanguageModel<short> languageModel(vectorizer,
//                                       clusteringMethod,
//                                       spellingTranscription);
//
//    // TODO: amplitude normalization of the data sources could be useful - it can be done as a filter, by taking
//    // TODO: the highest value of the amplitude in each data source separately and then scaling it lineary to some global value
//    vector<DataSource<short>> dataSources;
//    dataSources.push_back(WaveFileDataSource<short>("/home/kacper/voice/dziadzio.wav", sampleLength));
//    dataSources.push_back(WaveFileDataSource<short>("/home/kacper/voice/dziadzio_1.wav", sampleLength));
//    dataSources.push_back(WaveFileDataSource<short>("/home/kacper/voice/dziadzio_2.wav", sampleLength));
////    dataSources.push_back(WaveFileDataSource<short>("/home/kacper/voice/dziadzio_2_normalized.wav", sampleLength));
////    dataSources.push_back(WaveFileDataSource<short>("/home/kacper/voice/samogloski.wav", sampleLength));
//
//    vector<string> transcriptions;
//    transcriptions.push_back("dziadzio");
//    transcriptions.push_back("dziadzio");
//    transcriptions.push_back("dziadzio");
////    transcriptions.push_back("dziadzio");
////    transcriptions.push_back("a e i");
//
//    languageModel.fit(dataSources, transcriptions);
//
//    std::cout << std::endl << "Predictions:" << std::endl;
//    languageModel.predict(dataSources[0]);
//    languageModel.predict(dataSources[1]);
//    languageModel.predict(dataSources[2]);
//
//    auto newDS = WaveFileDataSource<short>("/home/kacper/voice/dziadzio_2_normalized.wav", sampleLength);
//    languageModel.predict(newDS);
//
////    try {
////        std::ofstream outputFile("model_gmm.lm");
////        languageModel.serialize(outputFile);
////        outputFile.close();
////
////        std::ifstream inputFile("model_gmm.lm");
////        LanguageModel<short> languageModelCopy(inputFile);
////        inputFile.close();
////
////        std::cout << std::endl << "Copy predictions:" << std::endl;
////        languageModelCopy.predict(dataSources[0]);
////        languageModelCopy.predict(dataSources[1]);
////        languageModelCopy.predict(dataSources[2]);
////    } catch (speech::exception::NullptrSerializationException& ex) {
////        std::cerr << "Serialization error: " << ex.what() << std::endl;
////    }
//
//    return 0;
}