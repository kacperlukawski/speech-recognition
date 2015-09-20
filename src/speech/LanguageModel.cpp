#include <istream>
#include "LanguageModel.h"

template<typename FrameType>
speech::LanguageModel<FrameType>::LanguageModel(std::istream &in) {
    uint32_t vectorizerTypeIdentifier;
    uint32_t clusteringMethodTypeIdentifier;
    uint32_t spellingTranscriptionTypeIdentifier;

    in.read((char *) &vectorizerTypeIdentifier, sizeof(uint32_t));
    switch (vectorizerTypeIdentifier) {
        case vectorizer::MaxFrequencyVectorizer<FrameType>::TYPE_IDENTIFIER:
            vectorizer = std::shared_ptr<IVectorizer<FrameType>>(new vectorizer::MaxFrequencyVectorizer<FrameType>(in));
            break;
        case vectorizer::FormantVectorizer<FrameType>::TYPE_IDENTIFIER:
            vectorizer = std::shared_ptr<IVectorizer<FrameType>>(new vectorizer::FormantVectorizer<FrameType>(in));
            break;
        case vectorizer::ThirdsPowerVectorizer<FrameType>::TYPE_IDENTIFIER:
            vectorizer = std::shared_ptr<IVectorizer<FrameType>>(new vectorizer::ThirdsPowerVectorizer<FrameType>(in));
            break;
        case vectorizer::MFCCVectorizer<FrameType>::TYPE_IDENTIFIER:
            vectorizer = std::shared_ptr<IVectorizer<FrameType>>(new vectorizer::MFCCVectorizer<FrameType>(in));
            break;
        default:
            throw exception::NullptrSerializationException();
    }

    in.read((char *) &clusteringMethodTypeIdentifier, sizeof(uint32_t));
    switch (clusteringMethodTypeIdentifier) {
        case clustering::KMeans::TYPE_IDENTIFIER:
            clusteringMethod = std::shared_ptr<clustering::IClusteringMethod>(new clustering::KMeans(in));
            break;
        case clustering::GaussianMixtureModel::TYPE_IDENTIFIER:
            clusteringMethod = std::shared_ptr<clustering::IClusteringMethod>(new clustering::GaussianMixtureModel(in));
            break;
        default:
            throw exception::NullptrSerializationException();
    }

    in.read((char *) &spellingTranscriptionTypeIdentifier, sizeof(uint32_t));
    switch (spellingTranscriptionTypeIdentifier) {
        case spelling::HMM::TYPE_IDENTIFIER:
            spellingTranscription = std::shared_ptr<spelling::ISpellingTranscription>(new spelling::HMM(in));
            break;
        default:
            throw exception::NullptrSerializationException();
    }
}

template<typename FrameType>
speech::LanguageModel<FrameType>::LanguageModel(shared_ptr<IVectorizer<FrameType>> vectorizer,
                                                shared_ptr<clustering::IClusteringMethod> clusteringMethod,
                                                shared_ptr<spelling::ISpellingTranscription> spellingTranscription)
        : vectorizer(vectorizer), clusteringMethod(clusteringMethod), spellingTranscription(spellingTranscription) {
}

template<typename FrameType>
void speech::LanguageModel<FrameType>::fit(vector<DataSource<FrameType>> &dataSources,
                                           vector<std::string> &transcriptions) {
    IFrequencyTransform<short> *fft = new FastFourierTransform<FrameType>();

    vector<valarray<double>> vectors;
    for (auto it = dataSources.begin(); it != dataSources.end(); it++) {
        std::vector<std::valarray<double>> vec = this->vectorizer->vectorize(*it);
        vectors.insert(vectors.end(), vec.begin(), vec.end());
    }

    // fit the clustering method to divide the dataset into groups
    clusteringMethod->fit(vectors);
    vectors.clear();

    // load single vectors from data source, cluster them and teach
    // spelling transcription module how to connect them with letters
    int sourceFilesSize = dataSources.size();
    for (int i = 0; i < sourceFilesSize; i++) {
        DataSource<FrameType> &dataSource = dataSources.at(i);
        std::string &transcription = transcriptions.at(i);

        vector<valarray<double>> samplesVectors = vectorizer->vectorize(dataSource); // vectorizer->vectorize(samples);

        std::vector<int> predictedLabels;
        for (valarray<double>& vector : samplesVectors) {
            int label = clusteringMethod->predict(vector);
            predictedLabels.push_back(label);
            std::cout << label << ' ';
        }

        std::cout << " (size = " << predictedLabels.size() << ")" << std::endl;

        spellingTranscription->fit(predictedLabels, transcription);
        predictedLabels.clear();
    }

    delete fft;
}

template<typename FrameType>
std::string speech::LanguageModel<FrameType>::predict(DataSource<FrameType> &dataSource) {
    vector<int> predictedLabels;
    vector<valarray<double>> samplesVectors = vectorizer->vectorize(dataSource);
    for (valarray<double>& vector : samplesVectors) {
        int label = clusteringMethod->predict(vector);
        predictedLabels.push_back(label);
        std::cout << label << ' ';
    }

    std::cout << " (size = " << predictedLabels.size() << ")" << std::endl;

    return spellingTranscription->predict(predictedLabels);
}

template<typename FrameType>
void speech::LanguageModel<FrameType>::serialize(std::ostream &out) const {
    if (vectorizer == nullptr) {
        throw exception::NullptrSerializationException("Vectorizer is set to NULL");
    } else {
        vectorizer->serialize(out);
    }

    if (clusteringMethod == nullptr) {
        throw exception::NullptrSerializationException("Clustering method is set to NULL");
    } else {
        // write the type identifier
        clusteringMethod->serialize(out);
    }

    if (spellingTranscription == nullptr) {
        throw exception::NullptrSerializationException("Spelling transcription is set to NULL");
    } else {
        // write the type identifier
        spellingTranscription->serialize(out);
    }
}

template
class speech::LanguageModel<short int>;