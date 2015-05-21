//
// Created by kacper on 18.05.15.
//

#include "MaximumDistance.h"

double speech::metric::MaximumDistance::operator()(const std::valarray<double> &v1,
                                                   const std::valarray<double> &v2) {
    std::valarray<double> diff = std::abs(v1 - v2);
    return diff.max();
}
