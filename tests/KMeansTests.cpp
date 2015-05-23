#include <string>
#include <memory>
#include <fstream>

#include "gtest/gtest.h"

#include "../src/speech/clustering/KMeans.h"
#include "../src/speech/clustering/exception/TooLessVectorsException.h"

using speech::clustering::KMeans;

#include "../src/speech/spelling/HMM.h"

using speech::spelling::HMM;

#include "../src/speech/LanguageModel.h"
#include "../src/speech/exception/NullptrSerializationException.h"

using speech::LanguageModel;

#include "../src/speech/vectorizer/IVectorizer.h"
#include "../src/speech/vectorizer/MaxFrequencyVectorizer.h"

using speech::vectorizer::IVectorizer;
using speech::vectorizer::MaxFrequencyVectorizer;

/**
 * KMeans - test case
 * XORTest - test name
 */
TEST(KMeans, XORTest) {
    // it is impossible to use KMeans for XOR problem
    // (vectors [0.0, 0.0] and [1.0, 1.0] are not the closest ones)
    // this test was only created to be some kind of an example
    // how to write a test of other things
    std::vector<std::valarray<double>> vectors;
    vectors.push_back(std::valarray<double>({0.0, 0.0}));
    vectors.push_back(std::valarray<double>({0.0, 1.0}));
    vectors.push_back(std::valarray<double>({1.0, 0.0}));
    vectors.push_back(std::valarray<double>({1.0, 1.0}));

    KMeans *kMeansPtr = new KMeans(2, 2);
    kMeansPtr->fit(vectors);

    int zeroVectorLabel = kMeansPtr->predict(std::valarray<double>({0.0, 0.0}));
    int firstVectorLabel = kMeansPtr->predict(std::valarray<double>({0.0, 1.0}));
    int twoVectorLabel = kMeansPtr->predict(std::valarray<double>({1.0, 0.0}));
    int threeVectorLabel = kMeansPtr->predict(std::valarray<double>({1.0, 1.0}));

    // ASSERT_EQ(zeroVectorLabel, threeVectorLabel);
    // ASSERT_EQ(firstVectorLabel, twoVectorLabel);

    // ASSERT_NE(zeroVectorLabel, firstVectorLabel);
    // ASSERT_NE(zeroVectorLabel, twoVectorLabel);
    // ASSERT_NE(threeVectorLabel, firstVectorLabel);
    // ASSERT_NE(threeVectorLabel, twoVectorLabel);
}

TEST(KMeans, KCentroidsTest) {
    // this test checks if the number of generated centroids
    // is proper - whenever we set up the KMeans method
    // to find K clusters and the data is properly
    // arranged, we should get all K clusters
    std::vector<std::valarray<double>> vectors;
    vectors.push_back(std::valarray<double>({0.0, 0.0}));
    vectors.push_back(std::valarray<double>({0.0, 1.0}));
    vectors.push_back(std::valarray<double>({1.0, 0.0}));
    vectors.push_back(std::valarray<double>({1.0, 1.0}));

    KMeans *kMeansPtr = new KMeans(5, 2);

    ASSERT_THROW(kMeansPtr->fit(vectors), speech::clustering::exception::TooLessVectorsException);
}

TEST(KMeans, RandomDataDistribution) {
    // this test checks if the clusters are the same for one
    // data set - KMeans should stop with exactly same clusters
    // when whole model stabilizes
    // @todo prepare the data set and proper tests
}

TEST(KMeans, SimpleCase) {
    std::vector<std::valarray<double>> leftSideVectors;
    leftSideVectors.push_back(std::valarray<double>({-1.0, 0.0, 0.0}));
    leftSideVectors.push_back(std::valarray<double>({-2.0, 0.0, 0.0}));

    std::vector<std::valarray<double>> rightSideVectors;
    rightSideVectors.push_back(std::valarray<double>({1.0, 0.0, 0.0}));
    rightSideVectors.push_back(std::valarray<double>({2.0, 0.0, 0.0}));

    std::vector<std::valarray<double>> vectors;
    vectors.insert(vectors.end(), rightSideVectors.begin(), rightSideVectors.end());
    vectors.insert(vectors.end(), leftSideVectors.begin(), leftSideVectors.end());

    KMeans *kMeansPtr = new KMeans(2, 3);
    kMeansPtr->fit(vectors);

    ASSERT_EQ(kMeansPtr->predict(leftSideVectors[0]), kMeansPtr->predict(leftSideVectors[1]));
    ASSERT_EQ(kMeansPtr->predict(rightSideVectors[0]), kMeansPtr->predict(rightSideVectors[1]));
    ASSERT_NE(kMeansPtr->predict(leftSideVectors[0]), kMeansPtr->predict(rightSideVectors[0]));
    ASSERT_NE(kMeansPtr->predict(leftSideVectors[0]), kMeansPtr->predict(rightSideVectors[1]));
}

//
// @todo extend the test to prepare a sequence of vectors, cluster
// them and run spelling transcription (check if the results
// returned by unserialized copy are the same)
//
TEST(KMeans, Serialization) {
    std::vector<std::valarray<double>> leftSideVectors;
    leftSideVectors.push_back(std::valarray<double>({-1.0, 0.0, 0.0}));
    leftSideVectors.push_back(std::valarray<double>({-2.0, 0.0, 0.0}));

    std::vector<std::valarray<double>> rightSideVectors;
    rightSideVectors.push_back(std::valarray<double>({1.0, 0.0, 0.0}));
    rightSideVectors.push_back(std::valarray<double>({2.0, 0.0, 0.0}));

    std::vector<std::valarray<double>> vectors;
    vectors.insert(vectors.end(), rightSideVectors.begin(), rightSideVectors.end());
    vectors.insert(vectors.end(), leftSideVectors.begin(), leftSideVectors.end());

    const int singleDataVectorDimension = 3;

    std::shared_ptr<IVectorizer<short>> vectorizerPtr = std::shared_ptr<IVectorizer<short>>(
            new MaxFrequencyVectorizer<short>(singleDataVectorDimension));

    std::shared_ptr<KMeans> kMeansPtr = std::shared_ptr<KMeans>(new KMeans(2, singleDataVectorDimension));
    kMeansPtr->fit(vectors);

    std::vector<int> leftLeftWordPhonems;
    leftLeftWordPhonems.push_back(kMeansPtr->predict(leftSideVectors[0]));
    leftLeftWordPhonems.push_back(kMeansPtr->predict(leftSideVectors[1]));

    std::vector<int> leftRightWordPhonems;
    leftRightWordPhonems.push_back(kMeansPtr->predict(leftSideVectors[0]));
    leftRightWordPhonems.push_back(kMeansPtr->predict(rightSideVectors[0]));

    std::vector<int> rightLeftWordPhonems;
    rightLeftWordPhonems.push_back(kMeansPtr->predict(rightSideVectors[0]));
    rightLeftWordPhonems.push_back(kMeansPtr->predict(leftSideVectors[0]));

    std::vector<int> rightRightWordPhonems;
    rightRightWordPhonems.push_back(kMeansPtr->predict(rightSideVectors[0]));
    rightRightWordPhonems.push_back(kMeansPtr->predict(rightSideVectors[0]));

    std::shared_ptr<HMM> hmmPtr = std::shared_ptr<HMM>(new HMM(2, 3));
    hmmPtr->fit(leftLeftWordPhonems, "aa");
    hmmPtr->fit(leftRightWordPhonems, "ab");
    hmmPtr->fit(rightLeftWordPhonems, "ba");
    hmmPtr->fit(rightRightWordPhonems, "bb");

    LanguageModel<short> *languageModel = new LanguageModel<short>(vectorizerPtr, kMeansPtr, hmmPtr);

    const std::string modelFileName = "kmeans_model.dat";

    try {
        std::ofstream outputFileStream(modelFileName);
        languageModel->serialize(outputFileStream);
        outputFileStream.close();

        std::ifstream inputFileStream(modelFileName);
        LanguageModel<short> *languageModelUnserialized = new LanguageModel<short>(inputFileStream);
        inputFileStream.close();

        std::shared_ptr<speech::vectorizer::IVectorizer<short>> vectorizer = languageModelUnserialized->getVectorizer();
        std::shared_ptr<speech::clustering::IClusteringMethod> clusteringMethod = languageModelUnserialized->getClusteringMethod();
        std::shared_ptr<speech::spelling::ISpellingTranscription> spellingTranscription = languageModelUnserialized->getSpellingTranscription();

        ASSERT_NE(vectorizer, nullptr);
        ASSERT_NE(clusteringMethod, nullptr);
        ASSERT_NE(spellingTranscription, nullptr);

        ASSERT_EQ(kMeansPtr->predict(leftSideVectors[0]), clusteringMethod->predict(leftSideVectors[0]));
        ASSERT_EQ(kMeansPtr->predict(leftSideVectors[1]), clusteringMethod->predict(leftSideVectors[1]));
        ASSERT_EQ(kMeansPtr->predict(rightSideVectors[0]), clusteringMethod->predict(rightSideVectors[0]));
        ASSERT_EQ(kMeansPtr->predict(rightSideVectors[0]), clusteringMethod->predict(rightSideVectors[1]));

        ASSERT_STREQ(hmmPtr->predict(leftLeftWordPhonems).c_str(), spellingTranscription->predict(leftLeftWordPhonems).c_str());
        ASSERT_STREQ(hmmPtr->predict(leftRightWordPhonems).c_str(), spellingTranscription->predict(leftRightWordPhonems).c_str());
        ASSERT_STREQ(hmmPtr->predict(rightLeftWordPhonems).c_str(), spellingTranscription->predict(rightLeftWordPhonems).c_str());
        ASSERT_STREQ(hmmPtr->predict(rightRightWordPhonems).c_str(), spellingTranscription->predict(rightRightWordPhonems).c_str());
    } catch (speech::exception::NullptrSerializationException& ex) {
        //std::cerr << ex.what() << std::endl;
        FAIL();
    }
}