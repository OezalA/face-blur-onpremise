#pragma once

#include "IFaceDetector.h"
#include <opencv2/objdetect.hpp>
#include <string>

// YuNet DNN face detector (OpenCV's cv::FaceDetectorYN). Implements the same
// IFaceDetector interface as HaarFaceDetector, so the rest of the app treats
// them identically -- this is what makes Haar <-> DNN switchable.
class DnnFaceDetector : public IFaceDetector {
public:
    // scoreThreshold: minimum confidence (0..1) to accept a detection.
    explicit DnnFaceDetector(const std::string& modelPath,
                             float scoreThreshold = 0.9f);

    // True if the model loaded successfully.
    bool isReady() const;

    std::vector<cv::Rect> detect(const cv::Mat& frame) override;

private:
    cv::Ptr<cv::FaceDetectorYN> detector_;
};