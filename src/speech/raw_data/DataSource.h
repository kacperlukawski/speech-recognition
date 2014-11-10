#ifndef DATASOURCE_H
#define DATASOURCE_H

#include "transform/IFrequencyTransform.h"

using speech::raw_data::transform::IFrequencyTransform;

#include "DataSample.h"

namespace speech {

    namespace raw_data {

        /**
         * This abstract class is a base for all data sources used in the application.
         * By data source we define the source of the audio signal.
         */
        template <typename FrameType>
        class DataSource {
        protected:
            DataSample<FrameType> * samples;
            IFrequencyTransform<FrameType> * frequencyDomainTransform;
        public:

        };

    }

}

#endif