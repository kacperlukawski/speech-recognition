#ifndef LANGUAGEMODEL_H
#define LANGUAGEMODEL_H

#include <vector>
#include <string>

#include "IStreamSerializable.h"
#include "exception/NullptrSerializationException.h"

#include "clustering/KMeans.h"

#include "spelling/HMM.h"

namespace speech {

    //
    // It is a representation of the language model which is built
    // from the clustering method and a method which converts
    // labeled signals into textual representation (we call it
    // spelling transcription)
    //
    class LanguageModel : public IStreamSerializable {
    public:
        LanguageModel(std::istream& in);

        LanguageModel(clustering::IClusteringMethod *clusteringMethod, spelling::ISpellingTranscription *spellingTranscription);

        inline clustering::IClusteringMethod* getClusteringMethod() {
            return clusteringMethod;
        }

        inline spelling::ISpellingTranscription* getSpellingTranscription() {
            return spellingTranscription;
        }

        virtual void serialize(std::ostream &out) const;
    protected:
        clustering::IClusteringMethod* clusteringMethod = nullptr;
        spelling::ISpellingTranscription* spellingTranscription = nullptr;
    private:
    };
}

#endif