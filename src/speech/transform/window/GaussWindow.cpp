#include "GaussWindow.h"
#include <cmath>

speech::transform::window::GaussWindow::GaussWindow(unsigned int windowSize, double sigma) : Window(windowSize) {
    this->sigma = sigma;
    window = std::vector<double>(windowSize);

    double tmp = (windowSize - 1) / 2;

    for (int i = 0; i < windowSize; ++i) {
        window[i] = exp(-0.5 * pow(((i - tmp) / (sigma * tmp)), 2));
    }

}

const double speech::transform::window::GaussWindow::getWindowMultiplier(unsigned int index) {
    return window[index];
}