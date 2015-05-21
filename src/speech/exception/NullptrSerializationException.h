#ifndef NULLPTRSERIALIZATIONEXCEPTION_H
#define NULLPTRSERIALIZATIONEXCEPTION_H

#include "SerializationException.h"

namespace speech {

    namespace exception {

        //
        // Exception thrown when we want to serialize NULL
        //
        class NullptrSerializationException : SerializationException  {
        public:
            NullptrSerializationException() : SerializationException() {}

            NullptrSerializationException(const char *msg) : SerializationException(msg) { }

            virtual const char *what() const _GLIBCXX_USE_NOEXCEPT {
                return ::std::length_error::what();
            }
        };

    }

}

#endif