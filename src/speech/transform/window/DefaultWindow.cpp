#include "DefaultWindow.h"


speech::transform::window::DefaultWindow::DefaultWindow(unsigned int windowSize) : Window(windowSize) {
}

std::vector<double>* speech::transform::window::DefaultWindow::createWindow(unsigned int windowSize) {
    return new std::vector<double>(windowSize, 1.0);
}
