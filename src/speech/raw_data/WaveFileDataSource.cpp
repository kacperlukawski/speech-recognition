#include <string.h>
#include <stdexcept>
#include "WaveFileDataSource.h"
#include <iostream>
#include <memory>

template<typename FrameType>
speech::raw_data::WaveFileDataSource<FrameType>::WaveFileDataSource(string _fileName)
        : DataSource<FrameType>() {
    this->fileName = _fileName;
    meta_data = new wav_header;
    readFromFile();
}

template<typename FrameType>
speech::raw_data::WaveFileDataSource<FrameType>::WaveFileDataSource(wav_header _meta_data) {
    meta_data = new wav_header;
    memcpy(meta_data, &_meta_data, sizeof(*meta_data));
}

template<typename FrameType>
speech::raw_data::WaveFileDataSource<FrameType>::~WaveFileDataSource() {
    if (meta_data != nullptr) {
        delete meta_data;
    }
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
        fwrite(it->getValues().get(), 1, it->getSize(), outfile);
    }
    fclose(outfile);
}

template<typename FrameType>
void speech::raw_data::WaveFileDataSource<FrameType>::readFromFile() {
    FILE *file = fopen(fileName.c_str(), "rb");

    if (file == NULL) {
        throw std::runtime_error(strerror(errno));
    }

    fread(meta_data, 1, sizeof(*meta_data), file);

    int numberOfRead = 0;

    while (!feof(file)) {
        std::shared_ptr<FrameType> buffer(new FrameType[BUFFER_SIZE], std::default_delete<FrameType[]>());
        numberOfRead = fread(buffer.get(), 1, BUFFER_SIZE, file);

        this->samples->push_back(DataSample<FrameType>(numberOfRead, buffer));
        buffer.reset();
    }

    fclose(file);
}

template
class speech::raw_data::WaveFileDataSource<unsigned char>;

template
class speech::raw_data::WaveFileDataSource<short int>;