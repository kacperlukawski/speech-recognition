//
// Created by kacper on 06.05.15.
//

#include <memory>
#include <cmath>

#include "NaiveSilenceDetector.h"

template<typename FrameType>
bool speech::detector::NaiveSilenceDetector<FrameType>::detected(FrequencySample<FrameType> &sample) const {
    int sampleSize = sample.getSize();
    double *amplitudePtr = sample.getAmplitude().get();

    double sampleAmplitudeSum = 0.0;
    for (int i = 0; i < sampleSize; i++) {
        sampleAmplitudeSum += amplitudePtr[i];
    }

    double sampleAmplitudeMean = sampleAmplitudeSum / sampleSize;

    double sampleAmplitudeSqrError = 0.0;
    for (int i = 0; i < sampleSize; i++) {
        sampleAmplitudeSqrError += pow(amplitudePtr[i] - sampleAmplitudeMean, 2.0);
    }

    double sampleAmplitudeVariance = sampleAmplitudeSqrError / sampleSize;
    double sampleAmplitudeStddev = sqrt(sampleAmplitudeVariance);

    // TODO: use thresholds passed as a model parameter
    return sampleAmplitudeMean < 0.5 && sampleAmplitudeStddev < 1.0;
}

template
class speech::detector::NaiveSilenceDetector<short>;
