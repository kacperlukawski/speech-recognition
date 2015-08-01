//
// Created by szymon on 01.08.15.
//

#ifndef SPEECH_RECOGNITION_DEFAULTWINDOW_H
#define SPEECH_RECOGNITION_DEFAULTWINDOW_H


#include "Window.h"

namespace speech {

    namespace transform {

        namespace window {

            class DefaultWindow : public Window {
            protected:
                virtual std::vector<double>* createWindow(unsigned int windowSize);
            public:
                DefaultWindow(unsigned int windowSize);
            };
        }
    }
}

#endif //SPEECH_RECOGNITION_DEFAULTWINDOW_H
