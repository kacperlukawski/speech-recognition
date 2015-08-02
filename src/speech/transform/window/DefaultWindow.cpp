#include "DefaultWindow.h"

speech::transform::window::DefaultWindow speech::transform::window::DefaultWindow::INSTANCE = speech::transform::window::DefaultWindow();

speech::transform::window::DefaultWindow* speech::transform::window::DefaultWindow::getInstance() {
    return &INSTANCE;
}

speech::transform::window::DefaultWindow::DefaultWindow() : Window(0) {
}

const double speech::transform::window::DefaultWindow::getWindowMultiplier(unsigned int index) {
    return 1.0;
}
