#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    // Open the default webcam (index 0; try 1 or 2 if it fails).
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cout << "Could not open the camera!" << std::endl;
        return -1;
    }

    // Load the Haar cascade once, outside the loop (loading per frame is wasteful).
    cv::CascadeClassifier faceCascade;
    if (!faceCascade.load("C:/opencv/build/etc/haarcascades/haarcascade_frontalface_default.xml")) {
        std::cout << "Could not load the cascade!" << std::endl;
        return -1;
    }

    // Run detection on a downscaled image for speed; detection cost scales
    // with pixel count, so 0.5x means ~4x fewer pixels and ~4x faster.
    const double DETECT_SCALE = 0.5;

    cv::Mat frame, gray, small;
    while (true) {
        // Mark the start of this frame to measure processing time.
        double t_start = cv::getTickCount();

        // Grab one frame from the camera.
        cap >> frame;
        if (frame.empty()) break;

        // Convert to grayscale (the detector works on intensity, not color).
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

        // Downscale the grayscale image, then detect on the smaller copy.
        cv::resize(gray, small, cv::Size(), DETECT_SCALE, DETECT_SCALE);

        std::vector<cv::Rect> faces;
        faceCascade.detectMultiScale(small, faces, 1.1, 4);

        // Draw a green rectangle around each face, scaling its coordinates
        // back up from the downscaled image to the full-resolution frame.
        for (const auto& face : faces) {
            cv::Rect scaled(cvRound(face.x / DETECT_SCALE),
                            cvRound(face.y / DETECT_SCALE),
                            cvRound(face.width / DETECT_SCALE),
                            cvRound(face.height / DETECT_SCALE));
            cv::rectangle(frame, scaled, cv::Scalar(0, 255, 0), 2);
        }

        // Compute this frame's processing time and convert it to FPS.
        double t_elapsed = (cv::getTickCount() - t_start) / cv::getTickFrequency(); // seconds
        double fps = 1.0 / t_elapsed;

        // Overlay FPS and milliseconds in the top-left corner.
        std::string overlay = cv::format("FPS: %.0f | %.0f ms", fps, t_elapsed * 1000.0);
        cv::putText(frame, overlay, cv::Point(10, 30),
                    cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 255, 0), 2);

        // Show the result.
        cv::imshow("Live Face Detection", frame);

        // Wait ~30 ms per frame; quit if 'q' is pressed during that time.
        if (cv::waitKey(30) == 'q') break;
    }

    // Clean shutdown.
    cap.release();
    cv::destroyAllWindows();
    return 0;
}
