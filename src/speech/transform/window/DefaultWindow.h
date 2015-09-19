#ifndef SPEECH_RECOGNITION_DEFAULTWINDOW_H
#define SPEECH_RECOGNITION_DEFAULTWINDOW_H


#include "Window.h"

namespace speech {

    namespace transform {

        namespace window {

            class DefaultWindow : public Window {
            private:
                static DefaultWindow INSTANCE;
            protected:
                virtual const double getWindowMultiplier(unsigned int index);
            public:
                DefaultWindow();

                static DefaultWindow* getInstance();
            };
        }
    }
}

#endif //SPEECH_RECOGNITION_DEFAULTWINDOW_H
