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

/**
 * Creates a two dimensional array of given type
 * @param d1 first dimension
 * @param d2 second dimension
 * @return two dimensional array
 */
template<typename T>
T **createArray(int d1, int d2) {
    T **array = new T *[d1];
    for (int i = 0; i < d1; ++i) {
        array[i] = new T[d2];
    }

    return array;
}

/**
 * Creates a three dimensional array of given type
 * @param d1 first dimension
 * @param d2 second dimension
 * @param d3 third dimension
 * @return three dimensional array
 */
template<typename T>
T ***createArray(int d1, int d2, int d3) {
    T ***array = new T **[d1];
    for (int i = 0; i < d1; ++i) {
        array[i] = new T *[d2];
        for (int j = 0; j < d2; ++j) {
            array[i][j] = new T[d3];
        }
    }

    return array;
}

/**
 * Destroys two dimensional array
 * @param d1 first dimension of the array
 */
template<typename T>
void removeArray(T **array, int d1) {
    for (int i = 0; i < d1; ++i) {
        delete[] array[i];
    }
    delete[] array;
}

/**
 * Destroys three dimensional array
 * @param d1 first dimension of the array
 * @param d2 second dimension of the array
 */
template<typename T>
void removeArray(T ***array, int d1, int d2) {
    for (int i = 0; i < d1; ++i) {
        for (int j = 0; j < d2; ++j) {
            delete[] array[i][j];
        }
        delete[] array[i];
    }
    delete[] array;
}

#endif //SPEECH_RECOGNITION_HELPERS_H
