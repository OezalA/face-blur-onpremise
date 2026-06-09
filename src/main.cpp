#include "HaarFaceDetector.h"
#include "DnnFaceDetector.h"
#include "FaceAnonymizer.h"

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <iostream>
#include <memory>

int main() {
    // Open the default webcam (index 0; try 1 or 2 if it fails).
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cout << "Could not open the camera!" << std::endl;
        return -1;
    }

    // --- The ONLY place Haar vs DNN is chosen. Thanks to IFaceDetector, the
    //     rest of the program does not change at all. ---
    const bool USE_DNN = true;

    std::unique_ptr<IFaceDetector> detector;
    std::string detectorName;
    if (USE_DNN) {
                auto dnn = std::make_unique<DnnFaceDetector>(
            std::string(MODELS_DIR) + "/face_detection_yunet_2023mar.onnx", 0.6f);
        if (!dnn->isReady()) {
            std::cout << "Could not load the DNN model!" << std::endl;
            return -1;
        }
        detector = std::move(dnn);
        detectorName = "DNN (YuNet)";
    } else {
        auto haar = std::make_unique<HaarFaceDetector>(
            "C:/opencv/build/etc/haarcascades/haarcascade_frontalface_default.xml");
        if (!haar->isReady()) {
            std::cout << "Could not load the cascade!" << std::endl;
            return -1;
        }
        detector = std::move(haar);
        detectorName = "Haar";
    }

    FaceAnonymizer anonymizer(*detector);

    cv::Mat frame;
    while (true) {
        // Mark the start of this frame to measure processing time.
        double t_start = cv::getTickCount();

        // Grab one frame from the camera.
        cap >> frame;
        if (frame.empty()) break;

        // Detect and blur all faces in place.
        anonymizer.anonymize(frame);

        // Compute this frame's processing time and convert it to FPS.
        double t_elapsed = (cv::getTickCount() - t_start) / cv::getTickFrequency();
        double fps = 1.0 / t_elapsed;

        // Overlay the active detector, FPS and milliseconds in the top-left.
        std::string overlay = cv::format("%s | FPS: %.0f | %.0f ms",
                                         detectorName.c_str(), fps, t_elapsed * 1000.0);
        cv::putText(frame, overlay, cv::Point(10, 30),
                    cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 255, 0), 2);

        // Show the result.
        cv::imshow("Live Face Anonymization", frame);

        // Wait ~30 ms per frame; quit if 'q' is pressed during that time.
        if (cv::waitKey(30) == 'q') break;
    }

    // Clean shutdown.
    cap.release();
    cv::destroyAllWindows();
    return 0;
}
