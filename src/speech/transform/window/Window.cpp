#include <stdexcept>
#include <string>
#include <sstream>
#include "Window.h"


speech::transform::window::Window::Window(unsigned int windowSize) {
    this->window = nullptr;
    this->windowSize = windowSize;
}

speech::transform::window::Window::~Window() {
    if (window != nullptr) {
        delete window;
    }
}

void speech::transform::window::Window::init(unsigned int windowSize) {
    this->window = createWindow(windowSize);

    if (this->window->size() != windowSize) {
        std::stringstream sstm;
        sstm << "Window size " << this->window->size() << " is incorrect. Required size: " << windowSize;
        throw std::logic_error(sstm.str());
    }
}

std::vector<double> *speech::transform::window::Window::getWindow() {
    if (window == nullptr) {
        init(windowSize);
    }
    return this->window;
}

double &speech::transform::window::Window::operator[](std::size_t el) {
    if (window == nullptr) {
        init(windowSize);
    }
    return (*window)[el];
}