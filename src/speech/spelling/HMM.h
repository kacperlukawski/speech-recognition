#ifndef HMM_H
#define HMM_H

#include "ISpellingTranscription.h"

namespace speech {

    namespace spelling {

        class HMM : public ISpellingTranscription {
        public:
            static const uint32_t TYPE_IDENTIFIER = 0x01000001;

            HMM();

            HMM(std::istream& in);

            virtual void fit(int *phonemes, string spelling);

            virtual string predict(int *phonemes);

            virtual void serialize(std::ostream &out) const;
        };

    }

}

#endif