#pragma once

#include "IFaceDetector.h"
#include <opencv2/objdetect.hpp>
#include <string>

// Haar cascade face detector. For speed it detects on a downscaled copy of the
// frame and scales the results back to full-resolution coordinates.
class HaarFaceDetector : public IFaceDetector {
public:
    // detectScale < 1.0 shrinks the image before detection (e.g. 0.5 = half).
    explicit HaarFaceDetector(const std::string& cascadePath,
                              double detectScale = 0.5);

    // True if the cascade file loaded successfully.
    bool isReady() const;

    std::vector<cv::Rect> detect(const cv::Mat& frame) override;

private:
    cv::CascadeClassifier cascade_;
    double detectScale_;
};
