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

                // TODO add more filters
            };
        }
    }
}


#endif //SPEECH_RECOGNITION_FIRFILTERBANK_H
