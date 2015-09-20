#include "SpellingAdjuster.h"

//
// Adjust the length of the string to given size.
//
// @return adjusted string
//
std::string speech::spelling::SpellingAdjuster::adjust(const std::string& text, const int length) {
    std::string result("");
    result.resize(length, ' ');

    unsigned long textLength = text.length();
    unsigned int eachLetterNumber = length / textLength;
    for (int pos = 0; pos < textLength; pos++) {
        for (int i = 0; i < eachLetterNumber; i++) {
            result[pos * eachLetterNumber + i] = text[pos];
        }
    }

    return result;
}