//
// Created by kacper on 15.06.15.
//

#ifndef SPEECH_RECOGNITION_HELPERS_H
#define SPEECH_RECOGNITION_HELPERS_H

#include <valarray>

/**
 * Computes a maximum value of each dimension of the vectors
 * @param v1 first array
 * @param v2 second array
 * @return array containing max values at each dimension
 */
std::valarray<double> maxItems(std::valarray<double> &v1, std::valarray<double> &v2);

/**
 * Computes a minimum value of each dimension of the vectors
 * @param v1 first array
 * @param v2 second array
 * @return array containing min values at each dimension
 */
std::valarray<double> minItems(std::valarray<double> &v1, std::valarray<double> &v2);

#endif //SPEECH_RECOGNITION_HELPERS_H
