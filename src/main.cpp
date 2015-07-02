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

#include "speech/helpers.h"



int main(int argc, char **argv) {


    return 0;
}