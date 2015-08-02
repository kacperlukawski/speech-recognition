#ifndef SPEECH_RECOGNITION_HAMMINGWINDOW_H
#define SPEECH_RECOGNITION_HAMMINGWINDOW_H

#include "Window.h"
#include <vector>

namespace speech {

    namespace transform {

        namespace window {

            class HammingWindow : public Window {
            private:
                std::vector<double> window;
            protected:
                virtual const double getWindowMultiplier(unsigned int index);
            public:
                HammingWindow(unsigned int windowSize);
            };
        }
    }
}

#endif //SPEECH_RECOGNITION_HAMMINGWINDOW_H
