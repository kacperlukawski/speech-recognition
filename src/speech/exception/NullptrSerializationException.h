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
        };

    }

}

#endif