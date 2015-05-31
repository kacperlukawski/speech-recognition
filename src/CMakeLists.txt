# add the executable "Main"
add_executable(
    Main
        main.cpp
        speech/LanguageModel.cpp
        speech/clustering/KMeans.cpp
        speech/clustering/GaussianMixtureModel.cpp
        speech/spelling/HMM.cpp
        speech/spelling/SpellingAdjuster.cpp
        speech/raw_data/filtering/MaxFrequenciesFilter.cpp
        speech/raw_data/filtering/EmphasisFilter.cpp
        speech/vectorizer/MaxFrequencyVectorizer.cpp
        speech/vectorizer/FormantVectorizer.cpp
        speech/vectorizer/MFCCVectorizer.cpp
        speech/metric/EuclideanDistance.cpp
        speech/metric/MaximumDistance.cpp
        speech/detector/NaiveSilenceDetector.cpp
        speech/vectorizer/ThirdsPowerVectorizer.cpp
        speech/transform/FastFourierTransform.cpp
        speech/transform/DiscreteFourierTransform.cpp
        speech/raw_data/WaveFileDataSource.cpp
        speech/raw_data/DataSource.cpp
        speech/raw_data/DataSample.cpp
        speech/raw_data/FrequencySample.cpp
        speech/raw_data/Spectrum.cpp)

# add VLFeat library usage
target_link_libraries(Main LINK_PUBLIC VLFeat)

# add the executable "Filters"
add_executable(
    Filters
        filters_test.cpp
        speech/LanguageModel.cpp
        speech/raw_data/DataSample.cpp
        speech/raw_data/FrequencySample.cpp
        speech/raw_data/WaveFileDataSource.cpp
        speech/raw_data/DataSource.cpp
        speech/clustering/KMeans.cpp
        speech/clustering/GaussianMixtureModel.cpp
        speech/spelling/HMM.cpp
        speech/spelling/SpellingAdjuster.cpp
        speech/transform/FastFourierTransform.cpp
        speech/transform/DiscreteFourierTransform.cpp
        speech/raw_data/filtering/MaxFrequenciesFilter.cpp
        speech/raw_data/filtering/EmphasisFilter.cpp
        speech/vectorizer/MaxFrequencyVectorizer.cpp
        speech/vectorizer/FormantVectorizer.cpp
        speech/vectorizer/MFCCVectorizer.cpp
        speech/metric/EuclideanDistance.cpp
        speech/metric/MaximumDistance.cpp
        speech/detector/NaiveSilenceDetector.cpp
        speech/vectorizer/ThirdsPowerVectorizer.cpp
        speech/raw_data/Spectrum.cpp)

# add VLFeat library usage
target_link_libraries(Filters LINK_PUBLIC VLFeat)

# add the executable "Harmonics"
add_executable(
    Harmonics
        harmonics.cpp
        speech/LanguageModel.cpp
        speech/raw_data/DataSample.cpp
        speech/raw_data/FrequencySample.cpp
        speech/raw_data/WaveFileDataSource.cpp
        speech/raw_data/DataSource.cpp
        speech/clustering/KMeans.cpp
        speech/clustering/GaussianMixtureModel.cpp
        speech/spelling/HMM.cpp
        speech/spelling/SpellingAdjuster.cpp
        speech/transform/FastFourierTransform.cpp
        speech/transform/DiscreteFourierTransform.cpp
        speech/raw_data/filtering/MaxFrequenciesFilter.cpp
        speech/raw_data/filtering/EmphasisFilter.cpp
        speech/vectorizer/MaxFrequencyVectorizer.cpp
        speech/vectorizer/FormantVectorizer.cpp
        speech/vectorizer/HarmonicsVectorizer.cpp
        speech/vectorizer/MFCCVectorizer.cpp
        speech/metric/EuclideanDistance.cpp
        speech/metric/MaximumDistance.cpp
        speech/detector/NaiveSilenceDetector.cpp
        speech/vectorizer/ThirdsPowerVectorizer.cpp
        speech/raw_data/Spectrum.cpp)

# add VLFeat library usage
target_link_libraries(Harmonics LINK_PUBLIC VLFeat)

# add the executable "Harmonics filter"
add_executable(
    Harmonics_filter
        harmonics_filter.cpp
        speech/LanguageModel.cpp
        speech/raw_data/DataSample.cpp
        speech/raw_data/FrequencySample.cpp
        speech/raw_data/WaveFileDataSource.cpp
        speech/raw_data/DataSource.cpp
        speech/clustering/KMeans.cpp
        speech/clustering/GaussianMixtureModel.cpp
        speech/spelling/HMM.cpp
        speech/spelling/SpellingAdjuster.cpp
        speech/transform/FastFourierTransform.cpp
        speech/transform/DiscreteFourierTransform.cpp
        speech/raw_data/filtering/MaxFrequenciesFilter.cpp
        speech/raw_data/filtering/EmphasisFilter.cpp
        speech/vectorizer/MaxFrequencyVectorizer.cpp
        speech/vectorizer/FormantVectorizer.cpp
        speech/vectorizer/HarmonicsVectorizer.cpp
        speech/vectorizer/MFCCVectorizer.cpp
        speech/metric/EuclideanDistance.cpp
        speech/metric/MaximumDistance.cpp
        speech/detector/NaiveSilenceDetector.cpp
        speech/vectorizer/ThirdsPowerVectorizer.cpp
        speech/raw_data/Spectrum.cpp)

# add VLFeat library usage
target_link_libraries(Harmonics_filter LINK_PUBLIC VLFeat)