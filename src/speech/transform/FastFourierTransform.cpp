#include "FastFourierTransform.h"
#include <cmath>
#include <limits>

template<typename FrameType>
speech::transform::FastFourierTransform<FrameType>::FastFourierTransform() {
    MAX_VALUE = std::numeric_limits<FrameType>::max();
}

template<typename FrameType>
FrequencySample<FrameType> speech::transform::FastFourierTransform<FrameType>::transform(DataSample<FrameType> vector) {
    std::shared_ptr<FrameType> values = vector.getValues();
    valarray<complex<double>> comp(vector.getSize());

    for (int i = 0; i < vector.getSize(); ++i) {
        comp[i] = complex<double>(((double) values.get()[i]) / MAX_VALUE, 0.0);
    }

    fft(comp);

    std::shared_ptr<double> amplitude(new double[vector.getSize()], std::default_delete<double[]>());
    std::shared_ptr<double> phase(new double[vector.getSize()], std::default_delete<double[]>());

    for (int i = 0; i < vector.getSize(); ++i) {
        amplitude.get()[i] = calculateAmplitude(comp[i]);
        phase.get()[i] = calculatePhase(comp[i]);
    }

    return FrequencySample<FrameType>(vector.getSize(), amplitude, phase);
}

template<typename FrameType>
DataSample<FrameType> speech::transform::FastFourierTransform<FrameType>::reverseTransform(FrequencySample<FrameType> vector) {
    std::shared_ptr<double> amplitude = vector.getAmplitude();
    std::shared_ptr<double> phase = vector.getPhase();

    valarray<complex<double>> comp(vector.getSize());

    for (int i = 0; i < vector.getSize(); ++i) {
        comp[i] = calculateSignal(amplitude.get()[i], phase.get()[i]);
    }

    ifft(comp);

    std::shared_ptr<FrameType> values(new FrameType[vector.getSize()], std::default_delete<FrameType[]>());

    for (int i = 0; i < vector.getSize(); ++i) {
        values.get()[i] = (FrameType) (comp[i].real() * MAX_VALUE);
    }

    return DataSample<FrameType>(vector.getSize(), values);
}

// http://rosettacode.org/wiki/Fast_Fourier_transform#C.2B.2B
// Cooley–Tukey FFT (in-place)
template<typename FrameType>
void speech::transform::FastFourierTransform<FrameType>::fft(valarray<complex<double>> &x) {
    const size_t N = x.size();
    if (N <= 1) return;

    // divide
    valarray<complex<double>> even = x[std::slice(0, N / 2, 2)];
    valarray<complex<double>> odd = x[std::slice(1, N / 2, 2)];

    // conquer
    fft(even);
    fft(odd);

    // combine
    for (size_t k = 0; k < N / 2; ++k) {
        complex<double> t = std::polar(1.0, -2.0 * M_PI * k / N) * odd[k];
        x[k] = even[k] + t;
        x[k + N / 2] = even[k] - t;
    }

}

// inverse fft (in-place)
template<typename FrameType>
void speech::transform::FastFourierTransform<FrameType>::ifft(valarray<complex<double>> &x) {
    // conjugate the complex numbers
    x = x.apply(std::conj);

    // forward fft
    fft(x);

    // conjugate the complex numbers again
    x = x.apply(std::conj);

    // scale the numbers
    x /= x.size();
}

template
class speech::transform::FastFourierTransform<short>;

template
class speech::transform::FastFourierTransform<unsigned char>;

