#include <iostream>

#include "speech/raw_data/WaveFileDataSource.h"

using speech::raw_data::WaveFileDataSource;

#include "speech/raw_data/DataSample.h"

#include "speech/transform/IFrequencyTransform.h"

/**
 * This is an entry point of the application. It allows user to select the data source used
 * to get an input data, choose the methods of signal transformation into frequency domain
 * and spelling transcription.
 *
 * @todo create a logic
 */
int main(int argc, char** argv) {

    WaveFileDataSource<signed char> * sourceFilePtr = new WaveFileDataSource<signed char>("lorem ipsum");

    std::cout << "Hello world!" << std::endl;

    return 0;
}