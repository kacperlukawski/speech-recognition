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

int main(int argc, char **argv) {
    // Get the list of the words' transcriptions and assigned files
    // Load all the files and

    const int SAMPLE_LENGTH = 20; // in milliseconds
    const int SAMPLE_OFFSET = 10; // in milliseconds
    const int MFCC_BINS = 26; // number of Mel filters in a bank
    const int MFCC_CEPSTRAL_COEFFICIENTS = 12; // number of cepstral coefficients used for analysis
    const double MFCC_MIN_FREQUENCY = 64.0; // in Herzs
    const double MFCC_MAX_FREQUENCY = 10000.0; // in Herzs
    const int LEXICON_DIMENSIONALITY = 3 * (MFCC_CEPSTRAL_COEFFICIENTS + 1);
    const int LEXICON_GAUSSIANS = 4;

    HMMLexicon lexicon(LEXICON_DIMENSIONALITY, LEXICON_GAUSSIANS);
    MFCCVectorizer<short int> *mfccVectorizer = new MFCCVectorizer<short int>(MFCC_BINS, MFCC_CEPSTRAL_COEFFICIENTS,
                                                                              MFCC_MIN_FREQUENCY, MFCC_MAX_FREQUENCY,
                                                                              SAMPLE_LENGTH, SAMPLE_OFFSET);

    int filesNumber = 2;
    std::string filenames[] = {
            "/home/kacper/Test/sala_21.wav",
            "/home/kacper/Projects/speech-recognition/dataset/splitted_records/record11/sala.wav"
    };
    std::string transcription("s|a|l|a");

    // do the following process for each file from the collection
    for (int i = 0; i < filesNumber; ++i){
        WaveFileDataSource<short int> dataSource(filenames[i], SAMPLE_LENGTH);
        HMMLexicon::Observation utterance = mfccVectorizer->vectorize(dataSource);
        lexicon.addUtterance(utterance, transcription, "|");
    }

    lexicon.fit();

    // get the same set once again and test what the model guesses about each utterance
    for (int i = 0; i < filesNumber; ++i){
        WaveFileDataSource<short int> dataSource(filenames[i], SAMPLE_LENGTH);
        HMMLexicon::Observation utterance = mfccVectorizer->vectorize(dataSource);
        lexicon.predict(utterance);
    }

    delete mfccVectorizer;

    return 0;
}