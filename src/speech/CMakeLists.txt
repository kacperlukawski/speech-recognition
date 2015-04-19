# creates a library called Speech
add_library(
    Speech
        LanguageModel.cpp
        raw_data/DataSample.cpp
        raw_data/FrequencySample.cpp
        raw_data/WaveFileDataSource.cpp
        raw_data/DataSource.cpp
        clustering/KMeans.cpp
        spelling/HMM.cpp
        spelling/SpellingAdjuster.cpp
        transform/FastFourierTransform.cpp
raw_data/filtering/IFilter.h raw_data/filtering/MaxFrequenciesFilter.cpp raw_data/filtering/MaxFrequenciesFilter.h vectorizer/IVectorizer.h vectorizer/MaxFrequencyVectorizer.cpp vectorizer/MaxFrequencyVectorizer.h metric/IMetric.h metric/EuclideanDistance.cpp metric/EuclideanDistance.h)

target_include_directories(Speech PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})