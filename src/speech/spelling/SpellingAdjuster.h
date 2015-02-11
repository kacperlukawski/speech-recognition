#ifndef SPELLINGADJUSTER_H
#define SPELLINGADJUSTER_H

#include <string>

namespace speech {

    namespace spelling {

        /**
        * This class performs an adjustment of the length of given
        * word by duplicating the letters. It is done using the
        * language model (some letters last longer than another ones).
        */
        class SpellingAdjuster {

        public:
            std::string adjust(const std::string& text, const int length);
        protected:
        private:

        };

    }

}

#endif