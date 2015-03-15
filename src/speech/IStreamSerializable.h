#ifndef ISTREAMSERIALIZABLE_H
#define ISTREAMSERIALIZABLE_H

#include <ostream>

namespace speech {

    //
    // This class gives an interface for all classes which can be serialized.
    // All classes used in an language model should inherit from this one,
    // to make sure that it is possible to save the model.
    //
    class IStreamSerializable {
    public:
        //
        // Write an object to given stream
        //
        virtual void serialize(std::ostream &out) const = 0;

        friend std::ostream &operator<<(std::ostream &out, const IStreamSerializable &object) {
            object.serialize(out);
            return out;
        }
    };

}

#endif