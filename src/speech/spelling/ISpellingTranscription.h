#ifndef ISPELLINGTRANSCRIPTION_H
#define ISPELLINGTRANSCRIPTION_H

#include <vector>
#include <string>

namespace speech {

    namespace spelling {

        /**
         * This class delivers an interface for all methods which converts sequences of
         * labeled phonems into letters.
         */
        class ISpellingTranscription {
        public:
            virtual void fit(std::vector<int> phonems, std::string spelling) = 0;

            virtual std::string predict(std::vector<int> phonems) = 0;
        };

    }

}

#endif