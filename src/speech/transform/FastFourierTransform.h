#ifndef FASTFOURIERTRANSFORM_H
#define FASTFOURIERTRANSFORM_H

#include <valarray>
#include <complex.h>
#include "IFrequencyTransform.h"

using speech::raw_data::DataSample;
using speech::raw_data::FrequencySample;
using std::valarray;
using std::complex;

namespace speech {

    namespace transform {

        /**
        * This is an interface for all transforms which converts raw signal
        * into frequency domain.
        */
        template<typename FrameType>
        class FastFourierTransform : public IFrequencyTransform<FrameType> {

        public:
            FastFourierTransform();

            virtual FrequencySample<FrameType> transform(DataSample<FrameType> vector);

            virtual DataSample<FrameType> reverseTransform(FrequencySample<FrameType> vector);

        protected:
            void fft(valarray<complex<double >> &x);

            void ifft(valarray<complex<double>> &x);

        private:
            int MAX_VALUE;

            /**
             * Calculates an amplitude of the signal.
             * @param value a result of FFT
             *
             * @return double
             */
            inline double calculateAmplitude(std::complex<double> value) {
//                return value.real() * MAX_VALUE;
                return std::abs(value);
            }

            /**
             * Calculates a phase of the signal.
             * @param value a result of FFT
             *
             * @return double
             */
            inline double calculatePhase(std::complex<double> value) {
//                return value.imag() * MAX_VALUE;
                return std::arg(value);
            }

            /**
             * Calculates a signal using amplitude and phase.
             * @param amplitude
             * @param phase
             *
             * @return complex
             */
            inline std::complex<double> calculateSignal(double amplitude, double phase) {
//                return complex<double>(amplitude / MAX_VALUE, phase / MAX_VALUE);
                return std::polar(amplitude, phase);
            }
        };

    }

}

#endif