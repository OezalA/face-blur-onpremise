#pragma once

#include <opencv2/core.hpp>
#include <vector>

// Abstraction for any face-detection strategy (Haar today, DNN in Faz 5).
// The rest of the app depends on this interface, not on a concrete detector,
// so the detection method can change without touching the callers
// (Dependency Inversion + Open/Closed).
class IFaceDetector {
public:
    virtual ~IFaceDetector() = default;

    // Detect faces in a BGR frame; returns bounding boxes in full-resolution
    // frame coordinates.
    virtual std::vector<cv::Rect> detect(const cv::Mat& frame) = 0;
};
