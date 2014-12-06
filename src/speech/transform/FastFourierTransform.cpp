#include "FastFourierTransform.h"
#include <cmath>

template<typename FrameType>
FrequencySample<FrameType> speech::transform::FastFourierTransform<FrameType>::transform(DataSample<FrameType> vector) {
    FrameType *values = vector.getValues();
    valarray<complex<double>> comp(vector.getSize());

    for (int i = 0; i < vector.getSize(); ++i) {
        comp[i] = complex<double>(((double) values[i]), 0.0);
    }

    fft(comp);

    double amplitude[vector.getSize()];
    double phase[vector.getSize()];

    for (int i = 0; i < vector.getSize(); ++i) {
        amplitude[i] = comp[i].real();
        phase[i] = comp[i].imag();
    }

    return FrequencySample<FrameType>(vector.getSize(), amplitude, phase);
}
template<typename FrameType>
DataSample<FrameType> speech::transform::FastFourierTransform<FrameType>::reverseTransform(FrequencySample<FrameType> vector) {
    double* amplitude = vector.getAmplitude();
    double* phase = vector.getPhase();
    valarray<complex<double>> comp(vector.getSize());

    for (int i = 0; i < vector.getSize(); ++i) {
        comp[i] = complex<double>(amplitude[i], phase[i]);
    }

    ifft(comp);

    FrameType values[vector.getSize()];

    for (int i = 0; i < vector.getSize(); ++i) {
        values[i] = (FrameType) comp[i].real();
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
        complex<double> t = std::polar(1.0, -2 * M_PI * k / N) * odd[k];
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

template class speech::transform::FastFourierTransform<short>;