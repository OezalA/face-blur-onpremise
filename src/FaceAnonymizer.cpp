#include "FaceAnonymizer.h"

#include <opencv2/imgproc.hpp>

FaceAnonymizer::FaceAnonymizer(IFaceDetector& detector, int blurStrength)
    : detector_(detector), blurStrength_(blurStrength) {}

void FaceAnonymizer::anonymize(cv::Mat& frame) {
    const std::vector<cv::Rect> faces = detector_.detect(frame);

    for (cv::Rect box : faces) {
        // Clamp to the frame so a face near the edge can't push the region
        // outside the image (which would crash).
        box &= cv::Rect(0, 0, frame.cols, frame.rows);
        if (box.empty()) continue;

        // frame(box) is a view into the original frame, so blurring it in
        // place modifies the frame directly -- no paste-back needed.
        cv::Mat roi = frame(box);
        cv::GaussianBlur(roi, roi, cv::Size(blurStrength_, blurStrength_), 0);
    }
}
