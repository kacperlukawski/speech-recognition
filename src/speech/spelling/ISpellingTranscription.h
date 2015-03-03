#ifndef ISPELLINGTRANSCRIPTION_H
#define ISPELLINGTRANSCRIPTION_H

#include "../IStreamSerializable.h"

#include <vector>
#include <string>

namespace speech {

    namespace spelling {

        //
        // This class delivers an interface for all methods which converts sequences of
        // labeled phonems into letters.
        //
        class ISpellingTranscription : public IStreamSerializable {
        public:
            virtual void fit(std::vector<int> &phonems, const std::string &spelling) = 0;

            virtual std::string predict(std::vector<int> phonems) = 0;

            virtual void serialize(std::ostream &out) const = 0;
        };
    }

}

#endif