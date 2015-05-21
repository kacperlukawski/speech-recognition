//
// Created by kacper on 19.05.15.
//

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
                EmphasisFilter(double alpha) : alpha(alpha) {}

                virtual DataSample<FrameType> filter(const DataSample<FrameType> &sample);
            private:
                double alpha;
            };
        }

    }

}


#endif //SPEECH_RECOGNITION_EMPHASISFILTER_H
