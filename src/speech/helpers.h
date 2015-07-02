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

/**
 * Gets the vector containing all letters occurring in given collection
 * of words. The result does not contain duplicates an blank characters.
 * @param transcriptions vector of texts
 * @return vector containing all characters occurred in given texts
 */
std::vector<char> getUniqueLetters(std::vector<std::string> &transcriptions);

/**
 * Loads whole content of the given file and returns it as a string
 * @param filepath path of the file to be read
 * @return file content
 */
std::string getFileContent(const std::string &filepath);

/**
 * Replaces all occurrences of the given pattern
 * @param s string where we want to replace pattern in
 * @param search pattern to be replaces
 * @param replace replacement string
 */
void str_replace(std::string &s, const std::string &search, const std::string &replace);

#endif //SPEECH_RECOGNITION_HELPERS_H
