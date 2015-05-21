#include <iostream>
#include <algorithm>
#include <string.h>
#include <complex>
#include <list>

#include "speech/raw_data/WaveFileDataSource.h"

using speech::raw_data::WaveFileDataSource;

#include "speech/transform/IFrequencyTransform.h"
#include "speech/transform/FastFourierTransform.h"

using namespace speech::transform;

#include "IFrequencySampleFilter.h"
#include "speech/raw_data/filtering/MaxFrequenciesFilter.h"

using speech::raw_data::filtering::IFrequencySampleFilter;
using speech::raw_data::filtering::MaxFrequenciesFilter;

#include "speech/vectorizer/IVectorizer.h"
#include "speech/vectorizer/MaxFrequencyVectorizer.h"

using speech::vectorizer::IVectorizer;
using speech::vectorizer::MaxFrequencyVectorizer;

std::vector<int> getRange(const std::string &text) {
    std::string item;
    std::vector<int> tmp;
    std::stringstream stringstream(text);
    while (std::getline(stringstream, item, '-')) {
        tmp.push_back(std::stoi(item));
    }

    if (tmp.size() == 1) {
        return tmp;
    }

    int from = tmp.at(0) - 1;
    int to = tmp.at(1) - 1;
    std::vector<int> numbers;
    for (int i = from; i <= to; i++) {
        numbers.push_back(i);
    }
    return numbers;
}

int main(int argc, char **argv) {
    const int singleSampleLength = 25;

    IFrequencyTransform<short> *fft = new FastFourierTransform<short>();
    WaveFileDataSource<short> *dataSourcePtr = new WaveFileDataSource<short>("/home/kacper/voice/samogloski.wav",
                                                                             singleSampleLength);
    IVectorizer<short> *vectorizer = new MaxFrequencyVectorizer<short>(10);
    IFrequencySampleFilter<short> *filter = new MaxFrequenciesFilter<short>(20);

    auto begin = dataSourcePtr->getSamplesIteratorBegin();
    auto end = dataSourcePtr->getSamplesIteratorEnd();

    std::vector<FrequencySample<short> *> frequencySamples;

    int i = 0;
    for (auto innerIt = begin; innerIt != end; innerIt++) {
        FrequencySample<short> frequencySample = fft->transform(*innerIt);
        frequencySamples.push_back(new FrequencySample<short>(frequencySample));

//        std::vector<double> vector = vectorizer->vectorize(frequencySample);
//        for (auto it = vector.begin(); it != vector.end(); it++) {
//            std::cout << *it << " ";
//        }
//        std::cout << "\n";
//
//        FrequencySample<short> resultSample = filter->filter(frequencySample);
//        double *amplitudePtr = resultSample.getAmplitude().get();
//        for (int j = 0; j < resultSample.getSize(); j++) {
//            amplitudePtr[j] *= 1.0;
//        }
//
//        for (int j = 0; j < multiplier; j++) {
//            dataExportPtr->addSample(fft->reverseTransform(resultSample));
//        }

//        std::cout << "frame " << (++i) << " - size: " << resultSample.getSize() << std::endl;
    }


    std::string choice;
    std::vector<int> selectedSamples;
    int numberOfSamples = frequencySamples.size();
    std::string filename = "/home/kacper/voice/samogloski";
    while (true) {
        std::cout
                << "Has " << numberOfSamples << " samples. "
                << "Please select the one you want to add (1-" << numberOfSamples << " / exit): ";
        std::cin >> choice;

        if (choice.compare("exit") == 0) {
            break;
        }

        std::vector<int> range = getRange(choice);
        selectedSamples.insert(selectedSamples.end(), range.begin(), range.end());
        filename = filename.append("_" + choice);
    }

    filename = filename.append(".wav");

    auto metadata = dataSourcePtr->getMetaData();
    metadata.subchunk2_size /= frequencySamples.size();
    metadata.subchunk2_size *= selectedSamples.size();
    metadata.chunk_size = 36 + metadata.subchunk2_size;
    WaveFileDataSource<short> *dataExportPtr = new WaveFileDataSource<short>(metadata, singleSampleLength);
    for (auto it = selectedSamples.begin(); it != selectedSamples.end(); it++) {
        FrequencySample<short> *sample = frequencySamples.at(*it);
        dataExportPtr->addSample(fft->reverseTransform(*sample));
    }
    dataExportPtr->saveToFile(filename.c_str());

    delete dataSourcePtr;
    delete dataExportPtr;

    return 0;
}