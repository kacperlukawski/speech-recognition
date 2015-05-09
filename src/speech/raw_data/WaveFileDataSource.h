#ifndef WAVEFILEDATASOURCE_H
#define WAVEFILEDATASOURCE_H

#include <string>
#include "DataSource.h"

using std::string;
using std::shared_ptr;
using std::pair;

namespace speech {

    namespace raw_data {

        // more in https://ccrma.stanford.edu/courses/422/projects/WaveFormat
        struct wav_header {
            char chunk_id[4];
            int chunk_size;
            char format[4];
            char subchunk1_id[4];
            int subchunk1_size;
            short int audio_format;
            short int num_channels;
            int sample_rate;
            int byte_rate;
            short int block_align;
            short int bits_per_sample;
            char subchunk2_id[4];
            int subchunk2_size;
        };

        /**
        * This class represents the data source which reads the signal
        * from standard WAV file
        *
        * @todo because wave file can have more than one channel (stereo is probably the most common format nowadays), we need to have a way to convert it into one signal - the transform should work only on one stream, while stereo has actually two independent streams of the data
        */
        template<typename FrameType>
        class WaveFileDataSource : public DataSource<FrameType> {
        public:
            WaveFileDataSource(wav_header _meta_data);
            WaveFileDataSource(string _fileName);
            virtual ~WaveFileDataSource();
            void saveToFile(string _fileName);
            virtual wav_header getMetaData();

        protected:
            string fileName;
        private:
            wav_header *meta_data;

            void readFromFile(bool convertToMono);

            unsigned int getBufferSize();
            void showFileInfo();
            pair<shared_ptr<FrameType>, int> convertSamplesToMono(shared_ptr<FrameType> buffer,
                                                                  int bufferSize,
                                                                  int num_channels);

        };
    }

}

#endif