# set up Google Test framework and enable testing
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(
    Tests
        KMeansTests.cpp # tests of KMeans clustering method
        SpellingAdjusterTests.cpp # tests of SpellingAdjuster
)

target_link_libraries(Tests LINK_PUBLIC Speech gtest gtest_main)

add_test(ExecuteTests Tests)