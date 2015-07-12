#include <string.h>
#include <stdexcept>
#include <iostream>
#include <memory>

#include "WaveFileDataSource.h"

template<typename FrameType>
speech::raw_data::wav_header speech::raw_data::WaveFileDataSource<FrameType>::createPcmWavHeader(short numChannels,
                                                                                                 int sampleRate,
                                                                                                 short bitsPerSample) {
    wav_header header = {
            {'R', 'I', 'F', 'F'},                               //chunk_id
            36,                                                 //chunk_size
            {'W', 'A', 'V', 'E'},                               //format
            {'f', 'm', 't', ' '},                               //subchunk1_id
            16,                                                 //subchunk1_size
            1,                                                  //audio_format
            numChannels,                                        //num_channels
            sampleRate,                                         //sample_rate
            sampleRate * numChannels * bitsPerSample / 8,       //byte_rate
            numChannels * bitsPerSample / 8,                    //block_align
            bitsPerSample,                                      //bits_per_sample
            {'d', 'a', 't', 'a'},                               //subchunk2_id
            0                                                   //subchunk2_size
    };

    return header;
}

template<typename FrameType>
speech::raw_data::WaveFileDataSource<FrameType>::WaveFileDataSource(string _fileName, int sampleLength)
        : DataSource<FrameType>() {
    this->fileName = _fileName;
    this->sampleLength = sampleLength;
    meta_data = new wav_header;
    readFromFile(true);
    showFileInfo();
}

template<typename FrameType>
void speech::raw_data::WaveFileDataSource<FrameType>::showFileInfo() {
    std::cout << "meta_data size:\t\t\t\t" << sizeof(*meta_data) << std::endl;
    std::cout << "chunk size:\t\t\t\t" << meta_data->chunk_size << std::endl;
    std::cout << "subchunk1_size:\t\t\t" << meta_data->subchunk1_size << std::endl;
    std::cout << "subchunk2_size:\t\t\t" << meta_data->subchunk2_size << std::endl;
    std::cout << "audio_format:\t\t\t" << meta_data->audio_format << std::endl;
    std::cout << "num_channels:\t\t\t" << meta_data->num_channels << std::endl;
    std::cout << "sample_rate:\t\t\t" << meta_data->sample_rate << std::endl;
    std::cout << "byte_rate:\t\t\t\t" << meta_data->byte_rate << std::endl;
    std::cout << "block_align:\t\t\t" << meta_data->block_align << std::endl;
    std::cout << "bits_per_sample:\t\t" << meta_data->bits_per_sample << std::endl;
    std::cout << std::endl;
}

template<typename FrameType>
speech::raw_data::WaveFileDataSource<FrameType>::WaveFileDataSource(wav_header _meta_data, int sampleLength) {
    this->meta_data = new wav_header;
    memcpy(this->meta_data, &_meta_data, sizeof(*this->meta_data));
    this->sampleLength = sampleLength;
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

    showFileInfo();
    fwrite(meta_data, 1, sizeof(*meta_data), outfile);

    for (auto it = this->getSamplesIteratorBegin(); it != this->getSamplesIteratorEnd(); ++it) {
        fwrite(it->getValues().get(), 1, it->getSize() * sizeof(FrameType), outfile);
    }
    fclose(outfile);
}

template<typename FrameType>
void speech::raw_data::WaveFileDataSource<FrameType>::readFromFile(bool convertToMono) {
    FILE *file = fopen(fileName.c_str(), "rb");

    if (file == NULL) {
        throw std::runtime_error(strerror(errno));
    }

    fread(meta_data, 1, sizeof(*meta_data), file);

    const unsigned int BUFFER_SIZE = getBufferSize();

    int numberOfRead = 0;

    while (!feof(file)) {
        std::shared_ptr<FrameType> buffer(new FrameType[BUFFER_SIZE], std::default_delete<FrameType[]>());
        numberOfRead = fread(buffer.get(), 1, BUFFER_SIZE, file) / sizeof(FrameType);

        pair<shared_ptr<FrameType>, int> mono;
        if (convertToMono) {
            mono = convertSamplesToMono(buffer, numberOfRead, meta_data->num_channels);
        } else {
            mono = std::make_pair(buffer, numberOfRead);
        }

        this->samples->push_back(DataSample<FrameType>(mono.second, sampleLength, mono.first));
        buffer.reset(); // TODO: use offset and allow to change the windowing
    }

    //
    if (convertToMono && meta_data->num_channels > 1) {
        meta_data->subchunk2_size /= meta_data->num_channels;
        meta_data->chunk_size = 36 + meta_data->subchunk2_size;
        meta_data->byte_rate = meta_data->sample_rate * meta_data->bits_per_sample / 8;
        meta_data->block_align = meta_data->bits_per_sample / 8;
        meta_data->num_channels = 1;
    }

    fclose(file);
}

template<typename FrameType>
static FrameType arithemticMean(std::shared_ptr<FrameType> array, int startIndex, int endIndex) {
    int result = 0;
    for (int i = startIndex; i < endIndex; ++i) {
        result += array.get()[i];
    }
    result = result / (endIndex - startIndex);
    return (FrameType) result;
}

template<typename FrameType>
pair<shared_ptr<FrameType>, int> speech::raw_data::WaveFileDataSource<FrameType>::convertSamplesToMono(
        shared_ptr<FrameType> buffer, int bufferSize, int num_channels) {
    if (num_channels == 1) {
        return std::make_pair(buffer, bufferSize);
    }
    int newSize = bufferSize / num_channels;
    std::shared_ptr<FrameType> monoBuffer(new FrameType[newSize], std::default_delete<FrameType[]>());

    int monoBufferIndex = 0;
    for (int i = 0; i < bufferSize; i += num_channels) {
        monoBuffer.get()[monoBufferIndex] = arithemticMean<FrameType>(buffer, i, i + num_channels);
        ++monoBufferIndex;
    }

    return std::make_pair(monoBuffer, newSize);
}

template<typename FrameType>
unsigned int speech::raw_data::WaveFileDataSource<FrameType>::getBufferSize() {
    // TODO: fix wrong Fourier transform when buffer size is not a power of 2
    return 1024; //meta_data->byte_rate * sampleLength / 1000;
}

template<typename FrameType>
void speech::raw_data::WaveFileDataSource<FrameType>::addSample(DataSample<FrameType> sample) {
    DataSource<FrameType>::addSample(sample);

    int sampleSize = sample.getSize();
    int sizeToAdd = sampleSize * meta_data->num_channels * meta_data->bits_per_sample / 8;
    meta_data->chunk_size += sizeToAdd;
    meta_data->subchunk2_size += sizeToAdd;
}

template
class speech::raw_data::WaveFileDataSource<unsigned char>;

template
class speech::raw_data::WaveFileDataSource<short int>;