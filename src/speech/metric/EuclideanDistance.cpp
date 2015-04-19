#include "EuclideanDistance.h"

double speech::metric::EuclideanDistance::operator()(const std::valarray<double> &v1,
                                                     const std::valarray<double> &v2) {
    std::valarray<double> difference = v1 - v2;
    std::valarray<double> square = difference * difference;
    return sqrt(square.sum());
}
