#include "FirFilter.h"

template<typename FrameType>
DataSample<FrameType> speech::raw_data::filtering::FirFilter<FrameType>::filter(const DataSample<FrameType> &sample) {

    double y, tmp[firFilterData.numTaps];
    std::shared_ptr<FrameType> filteredSignalValues(new FrameType[sample.getSize()],
                                                    std::default_delete<FrameType[]>());
    std::shared_ptr<FrameType> signal = sample.getValues();


    for (int i = 0; i < firFilterData.numTaps; ++i) {
        tmp[i] = 0.0;
    }

    for (int i = 0; i < sample.getSize(); ++i) {

        for (int k = firFilterData.numTaps; k > 1; --k) {
            tmp[k - 1] = tmp[k - 2];
        }
        tmp[0] = signal.get()[i];

        y = 0.0;
        for (int k = 0; k < firFilterData.numTaps; ++k) {
            y += firFilterData.taps[k] * tmp[k];
        }
        filteredSignalValues.get()[i] = y;
    }

    //  delete[] tmp;

    DataSample<FrameType> result(sample.getSize(), sample.getLength(), filteredSignalValues);
    return result;
}

template
class speech::raw_data::filtering::FirFilter<short>;