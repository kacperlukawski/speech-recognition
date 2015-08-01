#include "HammingWindow.h"
#include <cmath>

speech::transform::window::HammingWindow::HammingWindow(unsigned int windowSize) : Window(windowSize) {
}

std::vector<double>* speech::transform::window::HammingWindow::createWindow(unsigned int windowSize) {
    std::vector<double>* window = new std::vector<double>(windowSize);

    for (int i = 0; i < windowSize; ++i) {
        (*window)[i] = 0.53836 - 0.46164 * cos((2 * M_PI * i) / (windowSize - 1));
    }

    return window;
}
