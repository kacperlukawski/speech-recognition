#ifndef SPEECH_RECOGNITION_IWINDOW_H
#define SPEECH_RECOGNITION_IWINDOW_H

namespace speech {

    namespace transform {

        namespace window {

            class Window {
            private:
                unsigned int windowSize;
            protected:
                virtual const double getWindowMultiplier(unsigned int index) = 0;

            public:
                Window(unsigned int windowSize);

                virtual ~Window();

                double operator[](unsigned int index);
            };
        }
    }

}

#endif //SPEECH_RECOGNITION_IWINDOW_H
