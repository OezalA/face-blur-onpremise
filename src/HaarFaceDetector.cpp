#include "HaarFaceDetector.h"

#include <opencv2/imgproc.hpp>

HaarFaceDetector::HaarFaceDetector(const std::string& cascadePath,
                                   double detectScale)
    : detectScale_(detectScale) {
    cascade_.load(cascadePath);
}

bool HaarFaceDetector::isReady() const {
    return !cascade_.empty();
}

std::vector<cv::Rect> HaarFaceDetector::detect(const cv::Mat& frame) {
    // Detection works on intensity, so convert to grayscale first.
    cv::Mat gray, small;
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

    // Detect on a downscaled copy: ~4x fewer pixels at 0.5x means ~4x faster.
    cv::resize(gray, small, cv::Size(), detectScale_, detectScale_);

    std::vector<cv::Rect> faces;
    cascade_.detectMultiScale(small, faces, 1.1, 4);

    // Scale the boxes back up to full-resolution frame coordinates.
    for (cv::Rect& face : faces) {
        face = cv::Rect(cvRound(face.x / detectScale_),
                        cvRound(face.y / detectScale_),
                        cvRound(face.width / detectScale_),
                        cvRound(face.height / detectScale_));
    }
    return faces;
}
