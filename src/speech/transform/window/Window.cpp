#include <stdexcept>
#include <string>
#include <sstream>
#include "Window.h"


speech::transform::window::Window::Window(unsigned int windowSize) {
    this->windowSize = windowSize;
}

speech::transform::window::Window::~Window() {
}

double speech::transform::window::Window::operator[](unsigned int index) {
    return getWindowMultiplier(index);
}