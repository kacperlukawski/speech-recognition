#ifndef ICLUSTERINGMETHOD_H
#define ICLUSTERINGMETHOD_H

namespace speech {

    namespace clustering {

        /**
         * This is an interface for all clustering methods used for phonemes labeling
         */
        class IClusteringMethod {
            virtual void fit(double **vectors, int *labels) = 0;

            virtual int predict(double *vector) = 0;
        };

    }

}

#endif