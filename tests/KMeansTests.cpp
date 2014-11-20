#include "gtest/gtest.h"

#include "../src/speech/clustering/KMeans.h"

using speech::clustering::KMeans;

/**
 * KMeans - test case
 * XORTest - test name
 */
TEST(KMeans, XORTest) {
    // it is impossible to use KMeans for XOR problem
    // this test was only created to be some kind of an example
    // how to write a test of other things

    std::list<double *> vectors(4);
    vectors.push_back(new double[2]{0.0, 0.0});
    vectors.push_back(new double[2]{0.0, 1.0});
    vectors.push_back(new double[2]{1.0, 0.0});
    vectors.push_back(new double[2]{1.0, 1.0});

    KMeans *kMeansPtr = new KMeans(2, 2);
    kMeansPtr->fit(vectors, nullptr);

    int zeroVectorLabel = kMeansPtr->predict(new double[2]{0.0, 0.0});
    int firstVectorLabel = kMeansPtr->predict(new double[2]{0.0, 1.0});
    int twoVectorLabel = kMeansPtr->predict(new double[2]{1.0, 0.0});
    int threeVectorLabel = kMeansPtr->predict(new double[2]{1.0, 1.0});


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
    // @todo prepare the data set and proper tests
}

TEST(KMeans, RandomDataDistribution) {
    // this test checks if the clusters are the same for one
    // data set - KMeans should stop with exactly same clusters
    // when whole model stabilizes
    // @todo prepare the data set and proper tests
}