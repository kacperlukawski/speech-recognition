#ifndef SPEECH_RECOGNITION_HAMMINGWINDOW_H
#define SPEECH_RECOGNITION_HAMMINGWINDOW_H

#include "Window.h"

namespace speech {

    namespace transform {

        namespace window {

            class HammingWindow : public Window {
            protected:
                virtual std::vector<double>* createWindow(unsigned int windowSize);
            public:
                HammingWindow(unsigned int windowSize);
            };
        }
    }
}

#endif //SPEECH_RECOGNITION_HAMMINGWINDOW_H
