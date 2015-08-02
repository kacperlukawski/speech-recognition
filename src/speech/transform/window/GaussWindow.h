//
// Created by szymon on 02.08.15.
//

#ifndef SPEECH_RECOGNITION_GAUSSWINDOW_H
#define SPEECH_RECOGNITION_GAUSSWINDOW_H

#include "Window.h"
#include <vector>

namespace speech {

    namespace transform {

        namespace window {

            class GaussWindow : public Window {
            private:
                std::vector<double> window;
                double sigma;
            protected:
                virtual const double getWindowMultiplier(unsigned int index);
            public:
                GaussWindow(unsigned int windowSize, double sigma);
            };
        }
    }
}

#endif //SPEECH_RECOGNITION_GAUSSWINDOW_H
