//
// Created by kacper on 18.05.15.
//

#include <iostream>

#include "speech/raw_data/WaveFileDataSource.h"
#include "speech/raw_data/filtering/MaxFrequenciesFilter.h"
#include "speech/raw_data/filtering/EmphasisFilter.h"

using namespace speech::raw_data::filtering;
using namespace speech::raw_data;

#include "speech/transform/FastFourierTransform.h"

using namespace speech::transform;

int main(int argc, char** argv) {
    const int maxFrequencies = 16;
    const int sampleLength = 250;
    char *inputFilename = "/home/kacper/voice/record_17.wav";
    char *outputFilename = "/home/kacper/voice/record_17_filtered.wav";

    WaveFileDataSource<short> dataSource(inputFilename, sampleLength);
    MaxFrequenciesFilter<short> maxFrequenciesFilter(maxFrequencies);
    EmphasisFilter<short> emphasisFilter(0.97);
    FastFourierTransform<short> fft;

    auto metadata = dataSource.getMetaData();
    WaveFileDataSource<short> output(metadata, sampleLength);

    for (auto it = dataSource.getSamplesIteratorBegin(); it != dataSource.getSamplesIteratorEnd(); it++) {
//        FrequencySample<short> frequencySample = fft.transform(*it);
//        FrequencySample<short> filteredFrequencySample = maxFrequenciesFilter.filter(frequencySample);
//        output.addSample(fft.reverseTransform(filteredFrequencySample));
        output.addSample(emphasisFilter.filter(*it));
    }

    output.saveToFile(outputFilename);
    return 0;
}