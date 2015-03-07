#ifndef DATASOURCE_H
#define DATASOURCE_H

#include <list>
#include <memory>
#include "DataSample.h"

using std::list;
using speech::raw_data::DataSample;

namespace speech {

    namespace raw_data {

        /**
        * This abstract class is a base for all data sources used in the application.
        * By data source we define the source of the audio signal.
        *
        */
        template<typename FrameType>
        class DataSource {
        protected:
            std::shared_ptr<list<DataSample<FrameType>>> samples;
        public:
            DataSource();
            virtual ~DataSource();

            virtual void addSample(DataSample<FrameType> sample);
            virtual typename list<DataSample<FrameType>>::iterator getSamplesIteratorBegin();
            virtual typename list<DataSample<FrameType>>::iterator getSamplesIteratorEnd();
        };

    }

}

#endif