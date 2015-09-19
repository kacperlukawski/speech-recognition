#include <complex>
#include "DiscreteFourierTransform.h"
#include "./window/DefaultWindow.h"

template<typename FrameType>
FrequencySample<FrameType> speech::transform::DiscreteFourierTransform<FrameType>::transform(
        const DataSample<FrameType> &vector) {
    return transform(vector, speech::transform::window::DefaultWindow::getInstance());
}

template<typename FrameType>
FrequencySample<FrameType> speech::transform::DiscreteFourierTransform<FrameType>::transform(
        const DataSample<FrameType> &vector, Window* window) {
    int vectorSize = vector.getSize();
    int vectorLength = vector.getLength();

    FrameType *values = vector.getValues().get();
    double *amplitude = new double[vectorSize];
    double *phase = new double[vectorSize];

    for (int k = 0; k < vectorSize; k++) {
        std::complex<double> value;
        for (int n = 0; n < vectorSize; n++) {
            value += std::exp(std::complex<double>(0.0, (- 2 * M_PI * k * n) / vectorSize)) * std::complex<double>(values[n] * (*window)[n], 0.0);
        }

        amplitude[k] = std::abs(value);
        phase[k] = std::arg(value);
    }

    return FrequencySample<FrameType>(vectorSize,
                                      vectorLength,
                                      std::shared_ptr<double>(amplitude, std::default_delete<double[]>()),
                                      std::shared_ptr<double>(phase, std::default_delete<double[]>()));
}

template<typename FrameType>
DataSample<FrameType> speech::transform::DiscreteFourierTransform<FrameType>::reverseTransform(
        const FrequencySample<FrameType> &vector) {
    int vectorSize = vector.getSize();
    int vectorLength = vector.getLength();

    FrameType *values = new FrameType[vectorSize];
    double *amplitude = vector.getAmplitude().get();
    double *phase = vector.getPhase().get();

    for (int n = 0; n < vectorSize; n++) {
        std::complex<double> value;
        for (int k = 0; k < vectorSize; k++) {
            value += std::complex<double>(amplitude[k], phase[k]) * std::exp(std::complex<double>(0.0, (2 * M_PI * n * k) / vectorSize));
        }

        value /= vectorSize;
        values[n] = (FrameType) std::real(value);
    }

    std::shared_ptr<FrameType> valuesPtr(values, std::default_delete<FrameType[]>());
    return DataSample<FrameType>(vectorSize,
                                 vectorLength,
                                 valuesPtr);
}

template
class speech::transform::DiscreteFourierTransform<short>;