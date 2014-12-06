#ifndef HMM_H
#define HMM_H

#include "ISpellingTranscription.h"

namespace speech {

    namespace spelling {

        class HMM : public ISpellingTranscription {
        public:
            virtual void fit(int *phonemes, string spelling);

            virtual string predict(int *phonemes);
        };

    }

}

#endif