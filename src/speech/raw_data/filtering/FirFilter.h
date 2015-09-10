//
// Created by szymon on 08.09.15.
//

#ifndef SPEECH_RECOGNITION_FIRFILTER_H
#define SPEECH_RECOGNITION_FIRFILTER_H

#include "IDataSampleFilter.h"
#include "FirFilterBank.h"

namespace speech {

    namespace raw_data {

        namespace filtering {

            template<typename FrameType>
            class FirFilter : public IDataSampleFilter<FrameType> {

            public:

                FirFilter(const FirFilterData ffd) : firFilterData(ffd) {}

                virtual DataSample<FrameType> filter(const DataSample<FrameType> &sample);

            private:
                FirFilterData firFilterData;
            };
        }
    }
}


#endif //SPEECH_RECOGNITION_FIRFILTER_H
