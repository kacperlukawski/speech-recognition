#ifndef ICLUSTERINGMETHOD_H
#define ICLUSTERINGMETHOD_H

#include "../IStreamSerializable.h"

#include <vector>
#include <valarray>

using std::vector;
using std::valarray;

namespace speech {

    namespace clustering {

        /**
         * This is an interface for all clustering methods used for phonemes labeling
         */
        class IClusteringMethod : public IStreamSerializable {
        public:
            virtual void fit(vector<valarray<double>> &vectors, vector<int> &labels) = 0;

            virtual int predict(const valarray<double> &vector) = 0;

            virtual void serialize(std::ostream &out) const = 0;
        };

    }

}

#endif