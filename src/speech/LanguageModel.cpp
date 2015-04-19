#include <istream>
#include "LanguageModel.h"

//
// Load the model from given input stream
//
// @todo it is necessary to recognize the objects which is our model built from
//
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
        default:
            throw exception::NullptrSerializationException();
    }

    in.read((char *) &clusteringMethodTypeIdentifier, sizeof(uint32_t));
    switch (clusteringMethodTypeIdentifier) {
        case clustering::KMeans::TYPE_IDENTIFIER:
            clusteringMethod = std::shared_ptr<clustering::IClusteringMethod>(new clustering::KMeans(in));
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
                                                std::shared_ptr<clustering::IClusteringMethod> clusteringMethod,
                                                std::shared_ptr<spelling::ISpellingTranscription> spellingTranscription)
        : vectorizer(vectorizer), clusteringMethod(clusteringMethod), spellingTranscription(spellingTranscription) {
}

template<typename FrameType>
void speech::LanguageModel<FrameType>::fit(vector<DataSource<FrameType>> &dataSources,
                                           vector<std::string> &transcriptions) {
    if (dataSources.size() != transcriptions.size()) {
        // TODO: throw an exception - the numbers need to match
    }

    IFrequencyTransform<short> *fft = new FastFourierTransform<FrameType>();

    vector<double *> vectors;
    for (auto it = dataSources.begin(); it != dataSources.end(); it++) {
        DataSource<FrameType> &dataSource = *it;

        auto begin = dataSource.getSamplesIteratorBegin();
        auto end = dataSource.getSamplesIteratorEnd();

        for (auto innerIt = begin; innerIt != end; innerIt++) {
            FrequencySample<FrameType> frequencySample = fft->transform(*innerIt);
            if (frequencySample.getSize() == 0) {
                continue;
            }

            std::valarray<double> vector = vectorizer->vectorize(frequencySample); // TODO: operate on valarray!

            double *rawDataVector = new double[vector.size()];
            for (int i = 0; i < vector.size(); i++) {
                rawDataVector[i] = vector[i];
            }

            vectors.push_back(rawDataVector);
        }
    }

    // fit the clustering method to divide the dataset into groups
    vector<int> labels; // for this purposes can be empty
    clusteringMethod->fit(vectors, labels);
    vectors.clear();

    // load single vectors from data source, cluster them and teach
    // spelling transcription module how to connect them with letters
    int sourceFilesSize = dataSources.size();
    for (int i = 0; i < sourceFilesSize; i++) {
        DataSource<FrameType> &dataSource = dataSources.at(i);
        std::string &transcription = transcriptions.at(i);

        auto begin = dataSource.getSamplesIteratorBegin();
        auto end = dataSource.getSamplesIteratorEnd();

        std::vector<int> predictedLabels;
        for (auto innerIt = begin; innerIt != end; innerIt++) {
            FrequencySample<FrameType> frequencySample = fft->transform(*innerIt);
            if (frequencySample.getSize() == 0) {
                continue;
            }

            // TODO: output of vectorizer should be the input of clustering
            std::valarray<double> vector = vectorizer->vectorize(frequencySample);

            double *rawDataVector = new double[vector.size()];
            for (int i = 0; i < vector.size(); i++) {
                rawDataVector[i] = vector[i];
            }

            int label = clusteringMethod->predict(rawDataVector);
            predictedLabels.push_back(label);

            std::cout << label;
        }

        std::cout << " (size = " << predictedLabels.size() << ")" << std::endl;

        spellingTranscription->fit(predictedLabels, transcription);
        predictedLabels.clear();
    }

    delete fft;
}

template<typename FrameType>
std::string speech::LanguageModel<FrameType>::predict(DataSource<FrameType> &dataSource) {
    const int singleDataVectorDimension = 40; // TODO: remove!
    IFrequencyTransform<short> *fft = new FastFourierTransform<FrameType>();

    std::vector<int> predictedLabels;
    auto begin = dataSource.getSamplesIteratorBegin();
    auto end = dataSource.getSamplesIteratorEnd();
    for (auto innerIt = begin; innerIt != end; innerIt++) {
        FrequencySample<short> frequencySample = fft->transform(*innerIt);
        if (frequencySample.getSize() == 0) {
            continue;
        }

        std::valarray<double> vector = vectorizer->vectorize(frequencySample); // TODO: the same...

        double *rawDataVector = new double[singleDataVectorDimension];
        for (int i = 0; i < singleDataVectorDimension; i++) {
            double value = 0.0;
            if (i < vector.size()) {
                value = vector[i];
            }

            rawDataVector[i] = value;
        }

        int label = clusteringMethod->predict(rawDataVector);
        if (predictedLabels.size() > 0 && predictedLabels.back() == label) {
            continue;
        }

        predictedLabels.push_back(label);
    }

    delete fft;

    return spellingTranscription->predict(predictedLabels);
}

//
// This method saves the model into given stream. It works in the following way:
// 1. Saves the type of the clustering method (uint32_t)
// 2. Saves the clustering method (by calling its serialize method)
// 3. Saves the type of the spelling transcription (uint32_t)
// 4. Saves the spelling transcription (by calling its serialize method)
//
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