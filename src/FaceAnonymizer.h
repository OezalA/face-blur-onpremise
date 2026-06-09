#pragma once

#include "IFaceDetector.h"
#include <opencv2/core.hpp>

// Anonymizes faces in a frame by blurring every region the detector reports.
// It holds no detection logic of its own: it depends on the IFaceDetector
// abstraction, so switching Haar for DNN requires no change here.
// This is the reusable core that will become the DLL boundary in Faz 6.
class FaceAnonymizer {
public:
    // blurStrength is the Gaussian kernel size; must be odd. Larger = stronger.
    FaceAnonymizer(IFaceDetector& detector, int blurStrength = 51);

    // Detect and blur all faces in the frame, in place.
    void anonymize(cv::Mat& frame);

private:
    IFaceDetector& detector_;
    int blurStrength_;
};
