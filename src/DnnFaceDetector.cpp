#include "DnnFaceDetector.h"

DnnFaceDetector::DnnFaceDetector(const std::string& modelPath,
                                 float scoreThreshold) {
    try {
        // The input size here is a placeholder; we set the real size per frame.
        detector_ = cv::FaceDetectorYN::create(
            modelPath, "", cv::Size(320, 320), scoreThreshold);
    } catch (const cv::Exception&) {
        detector_.release();  // leave empty so isReady() reports failure
    }
}

bool DnnFaceDetector::isReady() const {
    return detector_ != nullptr && !detector_.empty();
}

std::vector<cv::Rect> DnnFaceDetector::detect(const cv::Mat& frame) {
    std::vector<cv::Rect> result;
    if (!isReady() || frame.empty()) return result;

    // YuNet must be told the exact frame size before each detection.
    detector_->setInputSize(frame.size());

    // Output: one row per face, columns [x, y, w, h, 10 landmark values, score].
    cv::Mat faces;
    detector_->detect(frame, faces);

    // We only need the bounding box (first four columns) for blurring.
    for (int i = 0; i < faces.rows; ++i) {
        int x = cvRound(faces.at<float>(i, 0));
        int y = cvRound(faces.at<float>(i, 1));
        int w = cvRound(faces.at<float>(i, 2));
        int h = cvRound(faces.at<float>(i, 3));
        result.emplace_back(x, y, w, h);
    }
    return result;
}