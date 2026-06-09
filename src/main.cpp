#include "HaarFaceDetector.h"
#include "FaceAnonymizer.h"

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <iostream>

int main() {
    // Open the default webcam (index 0; try 1 or 2 if it fails).
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cout << "Could not open the camera!" << std::endl;
        return -1;
    }

    // Build the detection + anonymization pipeline. main() depends only on the
    // concrete types it wires together; the blur core depends on the abstraction.
    HaarFaceDetector detector(
        "C:/opencv/build/etc/haarcascades/haarcascade_frontalface_default.xml");
    if (!detector.isReady()) {
        std::cout << "Could not load the cascade!" << std::endl;
        return -1;
    }
    FaceAnonymizer anonymizer(detector);

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

        // Overlay FPS and milliseconds in the top-left corner.
        std::string overlay = cv::format("FPS: %.0f | %.0f ms", fps, t_elapsed * 1000.0);
        cv::putText(frame, overlay, cv::Point(10, 30),
                    cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 255, 0), 2);

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
