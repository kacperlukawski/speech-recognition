#ifndef ISTREAMSERIALIZABLE_H
#define ISTREAMSERIALIZABLE_H

#include <ostream>

namespace speech {

    /**
     * This class gives an interface for all classes which can be serialized.
     * All classes used in an language model should inherit from this one,
     * to make sure that it is possible to save the model.
     */
    class IStreamSerializable {
    public:
        
        /**
         * Writes an object to given stream
         * @param out output stream
         */
        virtual void serialize(std::ostream &out) const = 0;

        /**
         * Overloads stream << operator, to allow passing instances
         * of IStreamSerializable into streams;
         */
        friend std::ostream &operator<<(std::ostream &out, const IStreamSerializable &object) {
            object.serialize(out);
            return out;
        }
    };

}

#endif