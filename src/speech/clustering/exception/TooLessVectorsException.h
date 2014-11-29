#ifndef TOOLESSVECTORSEXCEPTION_H
#define TOOLESSVECTORSEXCEPTION_H

#include <stdexcept>

namespace speech {

    namespace clustering {

        namespace exception {

            class TooLessVectorsException : public ::std::length_error {
            public:
                TooLessVectorsException() : length_error("") {}
            };

        }

    }

}

#endif