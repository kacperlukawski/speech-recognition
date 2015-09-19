#ifndef SPEECH_RECOGNITION_EMPHASISFILTER_H
#define SPEECH_RECOGNITION_EMPHASISFILTER_H

#include "IDataSampleFilter.h"

namespace speech {

    namespace raw_data {

        namespace filtering {

            /**
             * This filter boosts the amount of energy in the high frequencies.
             * @see http://webapp.etsi.org/workprogram/Report_WorkItem.asp?wki_id=18820
             */
            template<typename FrameType>
            class EmphasisFilter : public IDataSampleFilter<FrameType> {
            public:
                /**
                 * Creates a filter instance with provided weight of the
                 * each previous sample, which will be substracted from current
                 * sample to emphasise the difference between them:
                 * u[n] = u[n] - alpha * u[n-1]
                 * @param alpha weight of the previous sample
                 */
                EmphasisFilter(double alpha) : alpha(alpha) {}

                virtual DataSample<FrameType> filter(const DataSample<FrameType> &sample);
            private:
                /** Weight of the previous sample */
                double alpha;
            };
        }

    }

}


#endif //SPEECH_RECOGNITION_EMPHASISFILTER_H
