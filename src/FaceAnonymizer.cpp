#include "FaceAnonymizer.h"

#include <opencv2/imgproc.hpp>

FaceAnonymizer::FaceAnonymizer(IFaceDetector& detector, int mosaicBlocks)
    : detector_(detector), mosaicBlocks_(mosaicBlocks) {}

void FaceAnonymizer::anonymize(cv::Mat& frame) {
    const std::vector<cv::Rect> faces = detector_.detect(frame);

    for (cv::Rect box : faces) {
        // Clamp to the frame so a face near the edge can't push the region
        // outside the image (which would crash).
        box &= cv::Rect(0, 0, frame.cols, frame.rows);
        if (box.empty()) continue;

        // Pixelate (mosaic): shrink the face region to a few cells, then scale
        // it back up with nearest-neighbor so each cell becomes a solid block.
        // frame(box) is a view, so this edits the frame in place.
        cv::Mat roi = frame(box);
        cv::Mat tiny;
        cv::resize(roi, tiny, cv::Size(mosaicBlocks_, mosaicBlocks_), 0, 0,
                   cv::INTER_LINEAR);
        cv::resize(tiny, roi, roi.size(), 0, 0, cv::INTER_NEAREST);
    }
}
