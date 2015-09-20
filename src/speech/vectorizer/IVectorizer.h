#ifndef _SPEECH_RECOGNITION_IVECTORIZER_H_
#define _SPEECH_RECOGNITION_IVECTORIZER_H_

#include <valarray>
#include <vector>

#include "../IStreamSerializable.h"

using speech::IStreamSerializable;

#include "../raw_data/DataSource.h"
#include "../raw_data/DataSample.h"
#include "../raw_data/FrequencySample.h"

using speech::raw_data::DataSource;
using speech::raw_data::DataSample;
using speech::raw_data::FrequencySample;

#include "../transform/FastFourierTransform.h"

using speech::transform::IFrequencyTransform;
using speech::transform::FastFourierTransform;

namespace speech {

    namespace vectorizer {

        template<typename FrameType>
        class IVectorizer : public IStreamSerializable {
        public:
            /**
             * Projects given sample into feature space. Each vector needs to have
             * exactly same size.
             * @param sample single sample to be vectorized
             *
             * @return the projection of vector in a feature space
             */
            virtual std::valarray<double> vectorize(FrequencySample<FrameType> &sample) = 0;

            /**
             * Projects all samples from given collection into feature space.
             * Each vector needs to have the same dimension.
             * @param samples a collection of frequency domain samples
             * @deprecated pass the data source instead
             * @return the projection of all vectors from given collection
             */
            virtual std::vector<std::valarray<double>> vectorize(std::vector<FrequencySample<FrameType>> &samples) {
                std::vector<std::valarray<double>> result;
                for (FrequencySample<FrameType> frame : samples) {
                    result.push_back(this->vectorize(frame));
                }

                return std::move(result);
            }

            /**
             * Projects all samples from given data source into feature space.
             * @param dataSource data source containing the samples
             *
             * @return collection of all vectorized samples
             */
            virtual std::vector<std::valarray<double>> vectorize(DataSource<FrameType>& dataSource) {
                std::vector<std::valarray<double>> vectors;
                for (auto it = dataSource.getSamplesIteratorBegin(); it != dataSource.getSamplesIteratorEnd(); ++it) {
                    FrequencySample<FrameType> frequencySample = frequencyTransform->transform(*it);
                    vectors.push_back(vectorize(frequencySample));
                }

                return std::move(vectors);
            }

            /**
             * Serializes the vectorizer into given stream.
             * @param out output stream
             */
            virtual void serialize(std::ostream &out) const = 0;

            /**
             * Gets the size of the output vector produced by the vectorizer.
             *
             * @return size of the produced vector
             */
            virtual int getVectorSize() const = 0;
        protected:
            /** Transform between time and frequency domain */
            IFrequencyTransform<FrameType>* frequencyTransform = new FastFourierTransform<FrameType>();
        };

    }

}

#endif //_SPEECH_RECOGNITION_IVECTORIZER_H_
