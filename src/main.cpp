#include <iostream>
#include <list>

#include "speech/raw_data/WaveFileDataSource.h"

using speech::raw_data::WaveFileDataSource;

#include "speech/raw_data/DataSample.h"

#include "speech/transform/IFrequencyTransform.h"

#include "speech/clustering/KMeans.h"

using speech::clustering::KMeans;

/**
* This is an entry point of the application. It allows user to select the data source used
* to get an input data, choose the methods of signal transformation into frequency domain
* and spelling transcription.
*
* @todo create a logic
*/
int main(int argc, char **argv) {
    WaveFileDataSource<unsigned char> *sourceFilePtr = new WaveFileDataSource<unsigned char>("lorem ipsum");

    KMeans *kMeansClusteringPtr = new KMeans(2, 2);

    std::cout << "Hello world!" << std::endl;

    return 0;
}