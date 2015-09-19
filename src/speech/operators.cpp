#include "operators.h"

std::ostream& operator<<(std::ostream& out, const std::valarray<double>& v) {
    int size = v.size();
    out << '[';
    for (int i = 0; i < size - 1; i++) {
        out << v[i] << ' ';
    }
    if (size > 0) {
        out << v[size - 1];
    }
    out << ']';

    return out;
}