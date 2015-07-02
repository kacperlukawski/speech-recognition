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

std::vector<char> getUniqueLetters(std::vector<std::string> &transcriptions) {
    std::vector<char> letters;
    for (auto it = transcriptions.begin(); it != transcriptions.end(); it++) {
        int length = (*it).size();
        for (int i = 0; i < length; i++) {
            char letter = (*it)[i];

            // blank characters are not included
            if (letter == ' ' || letter == '\t' || letter == '\n' || letter == '\r') {
                continue;
            }

            auto pos = std::find(letters.begin(), letters.end(), letter);
            if (pos != letters.end()) {
                continue;
            }

            letters.push_back(letter);
        }
    }

    return letters;
}

std::string getFileContent(const std::string &filepath) {
    std::ifstream file(filepath.c_str());
    std::string content = std::string(std::istreambuf_iterator<char>(file),
                                      std::istreambuf_iterator<char>());
    file.close();

    return content;
}

void str_replace(std::string &s, const std::string &search, const std::string &replace) {
    for (size_t pos = 0; ; pos += replace.length()) {
        pos = s.find(search, pos);
        if (pos == string::npos) break;

        s.erase(pos, search.length());
        s.insert(pos, replace);
    }
}