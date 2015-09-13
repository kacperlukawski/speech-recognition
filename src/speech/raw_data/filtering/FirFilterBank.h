//
// Created by szymon on 10.09.15.
//

#ifndef SPEECH_RECOGNITION_FIRFILTERBANK_H
#define SPEECH_RECOGNITION_FIRFILTERBANK_H

namespace speech {

    namespace raw_data {

        namespace filtering {

            struct FirFilterData {
                int numTaps;
                double *taps;
            };

            class FirFilterBank {

            public:

                /**
                 *{
                        gain: 1, // passband
                        ripple: 5dB,
                        start: 0Hz,
                        stop: 8000Hz
                    },
                            
                    {
                        gain: 0, // stopband
                        atten: -80dB,
                        start: 9000Hz,
                        stop: 22000Hz
                    }
                 */
                static const FirFilterData LOWPASS_44100_8000_5DB_80DB;

                /**
                 * {
                        gain: 1, // passband
                        ripple: 3dB,
                        start: 0Hz,
                        stop: 8000Hz
                    },
                            
                    {
                        gain: 0, // stopband
                        atten: -80dB,
                        start: 10000Hz,
                        stop: 22000Hz
                    }
                 */
                static const FirFilterData LOWPASS_44100_8000_3DB_80DB;

                /**
                 * {
                        gain: 1, // passband
                        ripple: 5dB,
                        start: 0Hz,
                        stop: 1000Hz
                    },
                            
                    {
                        gain: 0, // stopband
                        atten: -80dB,
                        start: 3000Hz,
                        stop: 22000Hz
                    }
                 */
                static const FirFilterData LOWPASS_44100_1000_3DB_80DB;

                /**
                 * {
                        gain: 0, // stopband
                        atten: -80dB,
                        start: 0Hz,
                        stop: 2000Hz
                    },
                            
                    {
                        gain: 1, // passband
                        ripple: 3dB,
                        start: 3000Hz,
                        stop: 22000Hz
                    }
                 */
                static const FirFilterData HIGHPASS_44100_3000_3DB_80DB;

                /**
                 * {
                        gain: 0, // stopband
                        atten: -80dB,
                        start: 0Hz,
                        stop: 7000Hz
                    },
                            
                    {
                        gain: 1, // passband
                        ripple: 3dB,
                        start: 8000Hz,
                        stop: 22000Hz
                    }
                 */
                static const FirFilterData HIGHPASS_44100_8000_3DB_80DB;

                /**
                 * {
                        gain: 0, // stopband
                        atten: -60dB,
                        start: 0Hz,
                        stop: 1000Hz
                    },
                            
                    {
                        gain: 1, // passband
                        ripple: 5dB,
                        start: 2000Hz,
                        stop: 5000Hz
                    },

                    {
                        gain: 0, // stopband
                        atten: -60dB,
                        start: 6000Hz,
                        stop: 22000Hz
                    }
                 */
                static const FirFilterData BANDPASS_44100_2000_5000_5DB_60DB;

                /**
                 * {
                        gain: 1, // passband
                        ripple: 3dB,
                        start: 0Hz,
                        stop: 1000Hz
                    },
                            
                    {
                        gain: 0, // stopband
                        atten: -60dB,
                        start: 2000Hz,
                        stop: 5000Hz
                    },

                    {
                        gain: 1, // passband
                        ripple: 3dB,
                        start: 6000Hz,
                        stop: 22000Hz
                    }
                 */
                static const FirFilterData BANDSTOP_44100_2000_5000_3DB_60DB;
            };
        }
    }
}


#endif //SPEECH_RECOGNITION_FIRFILTERBANK_H
