//
// Created by kacper on 15.06.15.
//

#include "helpers.h"

std::valarray<double> maxItems(std::valarray<double> &v1, std::valarray<double> &v2) {
    std::valarray<double> result(v1);
    if (v1.size() < v2.size()) {
        result = v2;
    }

    int minSize = std::min(v1.size(), v2.size());
    for (int i = 0; i < minSize; i++) {
        result[i] = std::max(v1[i], v2[i]);
    }

    return std::move(result);
}

std::valarray<double> minItems(std::valarray<double> &v1, std::valarray<double> &v2) {
    std::valarray<double> result(v1);
    if (v1.size() < v2.size()) {
        result = v2;
    }

    int minSize = std::min(v1.size(), v2.size());
    for (int i = 0; i < minSize; i++) {
        result[i] = std::min(v1[i], v2[i]);
    }

    return std::move(result);
}