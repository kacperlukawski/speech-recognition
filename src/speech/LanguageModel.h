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
#include "raw_data/filtering/IDataSampleFilter.h"
#include "raw_data/filtering/EmphasisFilter.h"

using speech::raw_data::DataSource;
using speech::raw_data::filtering::IDataSampleFilter;

#include "vectorizer/IVectorizer.h"
#include "vectorizer/MaxFrequencyVectorizer.h"
#include "vectorizer/FormantVectorizer.h"
#include "vectorizer/ThirdsPowerVectorizer.h"
#include "vectorizer/MFCCVectorizer.h"

using speech::vectorizer::IVectorizer;

#include "clustering/KMeans.h"
#include "clustering/GaussianMixtureModel.h"

#include "transform/IFrequencyTransform.h"
#include "transform/FastFourierTransform.h"
#include "transform/DiscreteFourierTransform.h"

using speech::transform::IFrequencyTransform;
using speech::transform::FastFourierTransform;
using speech::transform::DiscreteFourierTransform;

#include "spelling/HMM.h"

#include "detector/IDetector.h"
#include "detector/NaiveSilenceDetector.h"

using speech::detector::IDetector;
using speech::detector::NaiveSilenceDetector;

namespace speech {

    /**
     * It is a representation of the language model which is built
     * from the clustering method and a method which converts
     * labeled signals into textual representation (we call it
     * spelling transcription)
     *
     * @todo add methods allowing to add both data sample and frequency sample filters
     */
    template<typename FrameType>
    class LanguageModel : public IStreamSerializable {
    public:
        LanguageModel(std::istream& in);

        LanguageModel(shared_ptr<IVectorizer<FrameType>> vectorizer,
                      shared_ptr<clustering::IClusteringMethod> clusteringMethod,
                      shared_ptr<spelling::ISpellingTranscription> spellingTranscription);

        /**
         * Fits all model components
         * @param dataSources collection of data sources
         * @param transcriptions collection of transcriptions for each provided data source
         */
        void fit(vector<DataSource<FrameType>> &dataSources, vector<std::string> &transcriptions);

        /**
         * Predicts the sentence from given data source samples
         * @param dataSource referene to the data source
         *
         * @return predicted sentence
         */
        std::string predict(DataSource<FrameType> &dataSource);

        /**
         * Gets shared pointer to the vectorizer instance used by the model
         *
         * @return instance of the vectorizer
         */
        inline shared_ptr<IVectorizer<FrameType>> getVectorizer() {
            return vectorizer;
        }

        /**
         * Gets shared pointer to the clustering method used by the model
         *
         * @return instance of the clustering method
         */
        inline shared_ptr<clustering::IClusteringMethod> getClusteringMethod() {
            return clusteringMethod;
        }

        /**
         * Gets shared pointer to the spelling transcription used by the model
         *
         * @return instance of the spelling transcription
         */
        inline shared_ptr<spelling::ISpellingTranscription> getSpellingTranscription() {
            return spellingTranscription;
        }

        virtual void serialize(std::ostream &out) const;
    protected:
        shared_ptr<IVectorizer<FrameType>> vectorizer = nullptr;
        shared_ptr<clustering::IClusteringMethod> clusteringMethod = nullptr;
        shared_ptr<spelling::ISpellingTranscription> spellingTranscription = nullptr;
    private:
        shared_ptr<IDetector<FrameType>> silenceDetector =
                shared_ptr<IDetector<FrameType>>(new NaiveSilenceDetector<FrameType>());
        IDataSampleFilter<FrameType>* dataSampleFilter =
                new speech::raw_data::filtering::EmphasisFilter<FrameType>(0.97); // TODO: remove all hardcoded things
    };
}

#endif