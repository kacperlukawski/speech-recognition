#include <string.h>
#include <stdexcept>
#include "WaveFileDataSource.h"
#include <iostream>

template<typename FrameType>
speech::raw_data::WaveFileDataSource<FrameType>::WaveFileDataSource(string _fileName)
        : DataSource<FrameType>() {
    this->fileName = _fileName;
    meta_data = new wav_header;
    readFromFile();
}

template <typename FrameType>
speech::raw_data::WaveFileDataSource<FrameType>::WaveFileDataSource(wav_header* _meta_data) {
    this->meta_data = _meta_data;
}

template<typename FrameType>
speech::raw_data::WaveFileDataSource<FrameType>::~WaveFileDataSource() {
    delete meta_data;
    delete file;
}

template<typename FrameType>
speech::raw_data::wav_header speech::raw_data::WaveFileDataSource<FrameType>::getMetaData() {
    return *meta_data;
}

template<typename FrameType>
void speech::raw_data::WaveFileDataSource<FrameType>::saveToFile(string _fileName) {
    FILE *outfile = fopen(_fileName.c_str(), "wb");

    fwrite(meta_data, 1, sizeof(*meta_data), outfile);

    for (auto it = this->getSamplesIteratorBegin(); it != this->getSamplesIteratorEnd(); ++it) {
        fwrite(it->getValues(), 1, it->getSize(), outfile);
    }
    fclose(outfile);
}

template<typename FrameType>
void speech::raw_data::WaveFileDataSource<FrameType>::readFromFile() {
    file = fopen(fileName.c_str(), "rb");

    if (file == NULL) {
        throw std::runtime_error(strerror(errno));
    }

    fread(meta_data, 1, sizeof(*meta_data), file);

    FrameType* buffer;
    int numberOfRead = 0;

    while (!feof(file)) {
        buffer = new FrameType[BUFFER_SIZE];
        numberOfRead = fread(buffer, 1, BUFFER_SIZE, file);

        this->samples->push_back(DataSample<FrameType>(numberOfRead, buffer));
    }
}

template
class speech::raw_data::WaveFileDataSource<unsigned char>;

template
class speech::raw_data::WaveFileDataSource<short int>;