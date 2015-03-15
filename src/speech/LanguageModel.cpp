#include <istream>
#include "LanguageModel.h"

//
// Load the model from given input stream
//
// @todo it is necessary to recognize the objects which is our model built from
//
speech::LanguageModel::LanguageModel(std::istream& in) {
    uint32_t clusteringMethodTypeIdentifier;
    uint32_t spellingTranscriptionTypeIdentifier;

    in.read((char *) &clusteringMethodTypeIdentifier, sizeof(uint32_t));
    switch (clusteringMethodTypeIdentifier) {
        case clustering::KMeans::TYPE_IDENTIFIER:
            clusteringMethod = new clustering::KMeans(in);
            break;
        default:
            throw exception::NullptrSerializationException();
    }

    in.read((char *) &spellingTranscriptionTypeIdentifier, sizeof(uint32_t));
    switch (spellingTranscriptionTypeIdentifier) {
        case spelling::HMM::TYPE_IDENTIFIER:
            spellingTranscription = new spelling::HMM(in);
            break;
        default:
            throw exception::NullptrSerializationException();
    }
}

speech::LanguageModel::LanguageModel(clustering::IClusteringMethod *clusteringMethod, spelling::ISpellingTranscription *spellingTranscription)
        : clusteringMethod(clusteringMethod), spellingTranscription(spellingTranscription) {
}

//
// This method saves the model into given stream. It works in the following way:
// 1. Saves the type of the clustering method (uint32_t)
// 2. Saves the clustering method (by calling its serialize method)
// 3. Saves the type of the spelling transcription (uint32_t)
// 4. Saves the spelling transcription (by calling its serialize method)
//
void speech::LanguageModel::serialize(std::ostream &out) const {
    if (clusteringMethod == nullptr) {
        throw exception::NullptrSerializationException();
    } else {
        // write the type identifier
        clusteringMethod->serialize(out);
    }

    if (spellingTranscription == nullptr) {
        throw exception::NullptrSerializationException();
    } else {
        // write the type identifier
        spellingTranscription->serialize(out);
    }
}