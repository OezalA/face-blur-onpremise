#include "FaceBlurApi.h"

#include "DnnFaceDetector.h"
#include "FaceAnonymizer.h"

#include <opencv2/core.hpp>
#include <memory>

// The real object behind the opaque handle. It owns the detector and the
// anonymizer, so the model is loaded once and reused for every frame.
struct FaceBlurEngine {
    std::unique_ptr<IFaceDetector> detector;
    std::unique_ptr<FaceAnonymizer> anonymizer;
};

FaceBlurHandle FaceBlur_Create(const char* modelPath, float scoreThreshold,
                               int mosaicBlocks) {
    auto dnn = std::make_unique<DnnFaceDetector>(modelPath, scoreThreshold);
    if (!dnn->isReady()) {
        return nullptr;  // model failed to load -> tell the caller via NULL
    }

    auto engine = new FaceBlurEngine();
    engine->anonymizer = std::make_unique<FaceAnonymizer>(*dnn, mosaicBlocks);
    engine->detector = std::move(dnn);  // keep the detector alive in the engine
    return engine;                      // FaceBlurEngine* -> opaque void* handle
}

void FaceBlur_ProcessBGR(FaceBlurHandle handle, unsigned char* data,
                         int width, int height, int step) {
    if (handle == nullptr || data == nullptr) return;
    auto engine = static_cast<FaceBlurEngine*>(handle);

    // Wrap the caller's pixel buffer in a cv::Mat WITHOUT copying: same memory.
    // Editing this Mat edits the caller's buffer directly (zero-copy).
    cv::Mat frame(height, width, CV_8UC3, data, step);
    engine->anonymizer->anonymize(frame);
}

void FaceBlur_SetMosaicBlocks(FaceBlurHandle handle, int blocks) {
    if (handle == nullptr) return;
    auto engine = static_cast<FaceBlurEngine*>(handle);
    engine->anonymizer->setMosaicBlocks(blocks);
}

void FaceBlur_Destroy(FaceBlurHandle handle) {
    delete static_cast<FaceBlurEngine*>(handle);
}
