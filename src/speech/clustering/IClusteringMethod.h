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
         * This is an interface for all clustering methods
         */
        class IClusteringMethod : public IStreamSerializable {
        public:
            /**
             * Trains the classifier using provided set of vectors
             * @param vectors collection of vectors used in a training phase
             */
            virtual void fit(vector<valarray<double>> &vectors) = 0;

            /**
             * Predicts a label of given vector. It can be used only after training
             * the classifier using fit method.
             * @param vector vector to be classified
             */
            virtual int predict(const valarray<double> &vector) = 0;

            virtual void serialize(std::ostream &out) const = 0;
        };

    }

}

#endif