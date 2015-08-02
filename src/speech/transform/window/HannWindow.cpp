//
// Created by szymon on 02.08.15.
//

#include "HannWindow.h"
#include <cmath>

speech::transform::window::HannWindow::HannWindow(unsigned int windowSize) : Window(windowSize) {
    window = std::vector<double>(windowSize);

    for (int i = 0; i < windowSize; ++i) {
        window[i] = 0.5 * (1 - cos((2 * M_PI * i) / (windowSize - 1)));
    }

}

const double speech::transform::window::HannWindow::getWindowMultiplier(unsigned int index) {
    return window[index];
}