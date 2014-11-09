# CMakeLists files in this project can
# refer to the root source directory of the project as ${SPEECH_RECOGNITION_SOURCE_DIR} and
# to the root binary directory of the project as ${SPEECH_RECOGNITION_BINARY_DIR}.
cmake_minimum_required(VERSION 2.6)
project(SPEECH_RECOGNITION)

set(EXECUTABLE_OUTPUT_PATH ${SPEECH_RECOGNITION_SOURCE_DIR}/build)

add_subdirectory(src)