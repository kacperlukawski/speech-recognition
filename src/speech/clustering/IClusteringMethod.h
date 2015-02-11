#ifndef ICLUSTERINGMETHOD_H
#define ICLUSTERINGMETHOD_H

#include "../IStreamSerializable.h"

#include <vector>

namespace speech {

    namespace clustering {

        //
        // This is an interface for all clustering methods used for phonemes labeling
        //
        class IClusteringMethod : public IStreamSerializable {
        public:
            virtual void fit(std::vector<double *> vectors, std::vector<int> labels) = 0;

            virtual int predict(double *vector) = 0;

            virtual void serialize(std::ostream &out) const = 0;
        };

    }

}

#endif