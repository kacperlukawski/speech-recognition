#include "gtest/gtest.h"

#include <string>

using std::string;

#include "../src/speech/spelling/SpellingAdjuster.h"

using speech::spelling::SpellingAdjuster;

TEST(SpellingAdjuster, AdjustmentTest) {
    int expectedLength = 10;
    string originalString("lorem");

    SpellingAdjuster spellingAdjuster;

    ASSERT_STREQ("lloorreemm", spellingAdjuster.adjust(originalString, expectedLength).c_str());
}