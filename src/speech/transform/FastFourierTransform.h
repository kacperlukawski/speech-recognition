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
            virtual FrequencySample<FrameType> transform(DataSample<FrameType> vector);

            virtual DataSample<FrameType> reverseTransform(FrequencySample<FrameType> vector);

        protected:
            void fft(valarray<complex<double >> &x);
            void ifft(valarray<complex<double>> &x);
        };

    }

}

#endif