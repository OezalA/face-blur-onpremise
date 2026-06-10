#include "FaceBlurApi.h"

#include <iostream>
#include <string>
#include <vector>

int main() {
    const std::string modelPath =
        std::string(MODELS_DIR) + "/face_detection_yunet_2023mar.onnx";

    // 1. Create the engine (loads the model once).
    FaceBlurHandle h = FaceBlur_Create(modelPath.c_str(), 0.6f, 13);
    if (h == nullptr) {
        std::cout << "FAIL: FaceBlur_Create returned NULL (model not loaded)\n";
        return 1;
    }
    std::cout << "PASS: engine created\n";

    // 2. Process a dummy BGR frame -- exactly like C# will pass a raw byte
    //    buffer (no cv::Mat on the caller side). No face here, so the pixels
    //    won't change; we are only checking the call runs end to end.
    const int width = 640, height = 480, channels = 3;
    const int step = width * channels;
    std::vector<unsigned char> buffer(static_cast<size_t>(step) * height, 128);
    FaceBlur_ProcessBGR(h, buffer.data(), width, height, step);
    std::cout << "PASS: ProcessBGR ran without crashing\n";

    // 3. Defensive check: null handle / null data must NOT crash.
    FaceBlur_ProcessBGR(nullptr, buffer.data(), width, height, step);
    FaceBlur_ProcessBGR(h, nullptr, width, height, step);
    std::cout << "PASS: null arguments handled safely\n";

    // 4. Destroy the engine.
    FaceBlur_Destroy(h);
    std::cout << "PASS: engine destroyed\n";

    std::cout << "Smoke test OK\n";
    return 0;
}
