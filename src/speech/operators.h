#ifndef SPEECH_RECOGNITION_OPERATORS_H
#define SPEECH_RECOGNITION_OPERATORS_H

#include <ostream>
#include <valarray>

/**
 * Overloaded operator for displaying a std::valarray into streams
 * @param out stream
 * @param v std::valarray instance
 * @return reference to stream
 */
std::ostream& operator<<(std::ostream& out, const std::valarray<double>& v);

#endif //SPEECH_RECOGNITION_OPERATORS_H
