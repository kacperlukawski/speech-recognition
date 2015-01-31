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
        };

    }

}

#endif