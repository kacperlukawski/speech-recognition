#ifndef SERIALIZATIONEXCEPTION_H
#define SERIALIZATIONEXCEPTION_H

#include <stdexcept>

namespace speech {

    namespace exception {

        //
        // General serialization exception
        //
        class SerializationException : public ::std::length_error {
        public:
            SerializationException() : length_error("") {}

            SerializationException(const char *msg) : length_error(msg) { }

            virtual const char *what() const _GLIBCXX_USE_NOEXCEPT {
                return ::std::length_error::what();
            }
        };
    }

}

#endif