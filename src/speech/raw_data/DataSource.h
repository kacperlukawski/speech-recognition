#ifndef DATASOURCE_H
#define DATASOURCE_H

#include <list>

using std::list;

#include "DataSample.h"

using speech::raw_data::DataSample;

namespace speech {

    namespace raw_data {

        /**
        * This abstract class is a base for all data sources used in the application.
        * By data source we define the source of the audio signal.
        *
        * @todo create shared interface for all data sources (probably iterators)
        */
        template<typename FrameType>
        class DataSource {
        protected:
            list<DataSample<FrameType>> *samples;
        public:
            DataSource();
        };

    }

}

#endif