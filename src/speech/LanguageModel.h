#ifndef LANGUAGEMODEL_H
#define LANGUAGEMODEL_H

#include <vector>
#include <memory>
#include <string>

using std::vector;
using std::shared_ptr;

#include "IStreamSerializable.h"
#include "exception/NullptrSerializationException.h"

#include "raw_data/DataSource.h"

using speech::raw_data::DataSource;

#include "vectorizer/IVectorizer.h"
#include "vectorizer/MaxFrequencyVectorizer.h"

using speech::vectorizer::IVectorizer;

#include "clustering/KMeans.h"

#include "transform/IFrequencyTransform.h"
#include "transform/FastFourierTransform.h"

using speech::transform::IFrequencyTransform;
using speech::transform::FastFourierTransform;

#include "spelling/HMM.h"

namespace speech {

    //
    // It is a representation of the language model which is built
    // from the clustering method and a method which converts
    // labeled signals into textual representation (we call it
    // spelling transcription)
    //
    template<typename FrameType>
    class LanguageModel : public IStreamSerializable {
    public:
        LanguageModel(std::istream& in);

        LanguageModel(shared_ptr<IVectorizer<FrameType>> vectorizer,
                      shared_ptr<clustering::IClusteringMethod> clusteringMethod,
                      shared_ptr<spelling::ISpellingTranscription> spellingTranscription);

        void fit(vector<DataSource<FrameType>> &dataSources, vector<std::string> &transcriptions);

        std::string predict(DataSource<FrameType> &dataSource);

        inline shared_ptr<IVectorizer<FrameType>> getVectorizer() {
            return vectorizer;
        }

        inline shared_ptr<clustering::IClusteringMethod> getClusteringMethod() {
            return clusteringMethod;
        }

        inline shared_ptr<spelling::ISpellingTranscription> getSpellingTranscription() {
            return spellingTranscription;
        }

        virtual void serialize(std::ostream &out) const;
    protected:
        shared_ptr<IVectorizer<FrameType>> vectorizer = nullptr;
        shared_ptr<clustering::IClusteringMethod> clusteringMethod = nullptr;
        shared_ptr<spelling::ISpellingTranscription> spellingTranscription = nullptr;
    private:
    };
}

#endif