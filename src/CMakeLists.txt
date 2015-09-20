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
        speech/raw_data/filtering/FirFilter.cpp
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
        speech/raw_data/Spectrum.cpp
        speech/initializer/RandomInitializer.cpp
        speech/initializer/KMeansInitializer.cpp
        speech/HMMLexicon.cpp
        speech/operators.cpp
        speech/helpers.cpp
        speech/transform/window/HammingWindow.cpp
        speech/transform/window/Window.cpp
        speech/transform/window/DefaultWindow.cpp
        speech/transform/window/HannWindow.cpp
        speech/transform/window/GaussWindow.cpp
        speech/raw_data/filtering/FirFilterBank.cpp)

# add VLFeat library usage
target_link_libraries(Main LINK_PUBLIC VLFeat JsonCpp)