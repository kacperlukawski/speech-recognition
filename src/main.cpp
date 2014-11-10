#include <iostream>

#include "speech/raw_data/DataSample.h"

int main(int argc, char** argv) {
    for (int i = 0; i < 1000000; i++) {
        signed char *tmp = new signed char[2];
        speech::raw_data::DataSample<signed char> *ds = new speech::raw_data::DataSample<signed char>(2, tmp);
    }

    std::cout << "Hello world!" << std::endl;

    return 0;
}