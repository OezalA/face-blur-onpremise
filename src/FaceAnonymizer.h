#pragma once

#include "IFaceDetector.h"
#include <opencv2/core.hpp>

// Anonymizes faces in a frame by pixelating (mosaic) every region the detector
// reports. It depends on the IFaceDetector abstraction, so switching Haar for
// DNN requires no change here. This is the reusable core for the DLL in Faz 6.
class FaceAnonymizer {
public:
    // mosaicBlocks: how many mosaic cells span each face. Fewer = chunkier.
    FaceAnonymizer(IFaceDetector& detector, int mosaicBlocks = 13);

    // Detect and pixelate all faces in the frame, in place.
    void anonymize(cv::Mat& frame);

private:
    IFaceDetector& detector_;
    int mosaicBlocks_;
};
