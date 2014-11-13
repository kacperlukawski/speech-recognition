# creates a library called Speech
add_library(
    Speech
        raw_data/DataSample.cpp
        raw_data/FrequencySample.cpp
        raw_data/DataSource.cpp
        raw_data/WaveFileDataSource.cpp
)

target_include_directories(Speech PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})