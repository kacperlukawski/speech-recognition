//
// Created by szymon on 02.08.15.
//

#ifndef SPEECH_RECOGNITION_HANNWINDOW_H
#define SPEECH_RECOGNITION_HANNWINDOW_H


#include "Window.h"
#include <vector>

namespace speech {

    namespace transform {

        namespace window {

            class HannWindow : public Window {
            private:
                std::vector<double> window;
            protected:
                virtual const double getWindowMultiplier(unsigned int index);
            public:
                HannWindow(unsigned int windowSize);
            };
        }
    }
}

#endif //SPEECH_RECOGNITION_HANNWINDOW_H
