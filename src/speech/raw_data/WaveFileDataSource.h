#ifndef WAVEFILEDATASOURCE_H
#define WAVEFILEDATASOURCE_H

#include <string>

using std::string;

#include "DataSource.h"

namespace speech {

    namespace raw_data {

        /**
        * This class represents the data source which reads the signal
        * from standard WAV file
        *
        * @todo implement file handling
        *
        * @todo because wave file can have more than one channel (stereo is probably the most common format nowadays), we need to have a way to convert it into one signal - the transform should work only on one stream, while stereo has actually two independent streams of the data
        */
        template<typename FrameType>
        class WaveFileDataSource : public DataSource<FrameType> {
        protected:
            string fileName;
        public:
            WaveFileDataSource(IFrequencyTransform<FrameType> *_frequencyDomainTransform, string _fileName);
        };

    }

}

#endif