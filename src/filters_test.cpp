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

#include "speech/raw_data/filtering/IFilter.h"
#include "speech/raw_data/filtering/MaxFrequenciesFilter.h"

using speech::raw_data::filtering::IFilter;
using speech::raw_data::filtering::MaxFrequenciesFilter;

int main(int argc, char **argv) {
    IFrequencyTransform<short> *fft = new FastFourierTransform<short>();

    WaveFileDataSource<short> *dataSourcePtr = new WaveFileDataSource<short>("/home/kacper/voice/linda.wav");
    WaveFileDataSource<short> *dataExportPtr = new WaveFileDataSource<short>(dataSourcePtr->getMetaData());

    IFilter<short> *filter = new MaxFrequenciesFilter<short>(5);

    auto begin = dataSourcePtr->getSamplesIteratorBegin();
    auto end = dataSourcePtr->getSamplesIteratorEnd();

    for (auto innerIt = begin; innerIt != end; innerIt++) {
        FrequencySample<short> frequencySample = fft->transform(*innerIt);
        FrequencySample<short> resultSample = filter->filter(frequencySample);
        dataExportPtr->addSample(fft->reverseTransform(resultSample));
    }

    dataExportPtr->saveToFile("/home/kacper/voice/linda_2.wav");

    delete dataSourcePtr;
    delete dataExportPtr;

    return 0;
}