#include "HammingWindow.h"
#include <cmath>

speech::transform::window::HammingWindow::HammingWindow(unsigned int windowSize) : Window(windowSize) {
    window = std::vector<double>(windowSize);

    for (int i = 0; i < windowSize; ++i) {
        window[i] = 0.53836 - 0.46164 * cos((2 * M_PI * i) / (windowSize - 1));
    }

}

const double speech::transform::window::HammingWindow::getWindowMultiplier(unsigned int index) {
    return window[index];
}