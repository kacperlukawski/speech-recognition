# CMakeLists files in this project can
# refer to the root source directory of the project as ${SPEECH_RECOGNITION_SOURCE_DIR} and
# to the root binary directory of the project as ${SPEECH_RECOGNITION_BINARY_DIR}.
cmake_minimum_required(VERSION 2.8.11)
project(SPEECH_RECOGNITION)

# enable c++11 standard
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# set "build" as a directory for executable files
set(EXECUTABLE_OUTPUT_PATH ${SPEECH_RECOGNITION_SOURCE_DIR}/build)

# add "src" directory to be used in compilation
add_subdirectory(src)