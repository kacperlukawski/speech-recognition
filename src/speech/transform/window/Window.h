#ifndef SPEECH_RECOGNITION_IWINDOW_H
#define SPEECH_RECOGNITION_IWINDOW_H

#include <vector>

namespace speech {

    namespace transform {

        namespace window {

            class Window {
            private:
                unsigned int windowSize;
                std::vector<double>* window;

                void init(unsigned int windowSize);

            protected:
                virtual std::vector<double>* createWindow(unsigned int windowSize) = 0;

            public:
                Window(unsigned int windowSize);

                virtual ~Window();

                std::vector<double>* getWindow();

                double &operator[](std::size_t el);
            };
        }
    }

}

#endif //SPEECH_RECOGNITION_IWINDOW_H
