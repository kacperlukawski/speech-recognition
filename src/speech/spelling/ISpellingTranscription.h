#ifndef ISPELLINGTRANSCRIPTION_H
#define ISPELLINGTRANSCRIPTION_H

#include <string>

using std::string;

namespace speech {

    namespace spelling {

        /**
         * This class delivers an interface for all methods which converts sequences of
         * labeled phonems into letters.
         */
        class ISpellingTranscription {
            virtual void fit(int * phonemes, string spelling) = 0;
            virtual string predict(int * phonemes) = 0;
        };

    }

}

#endif