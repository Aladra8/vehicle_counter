// #include "detector.hpp"
// #include <opencv2/imgproc.hpp>
// #include <opencv2/bgsegm.hpp>
// #include <opencv2/highgui.hpp>

// std::vector<cv::Rect> detect_vehicles(const cv::Mat& image, cv::Mat& gray, cv::Mat& fg_mask, cv::Mat& morph) {
//     static cv::Ptr<cv::BackgroundSubtractor> bg_subtractor = cv::createBackgroundSubtractorMOG2();

//     // Convert to grayscale
//     cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);

//     // Apply background subtraction
//     bg_subtractor->apply(gray, fg_mask);

//     // Remove shadows (127 = shadows in MOG2)
//     cv::threshold(fg_mask, fg_mask, 200, 255, cv::THRESH_BINARY);

//     // Morphological cleanup
//     cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
//     cv::morphologyEx(fg_mask, morph, cv::MORPH_CLOSE, kernel);

//     // Find contours
//     std::vector<std::vector<cv::Point>> contours;
//     cv::findContours(morph, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

//     std::vector<cv::Rect> boxes;
//     for (const auto& contour : contours) {
//         cv::Rect rect = cv::boundingRect(contour);
//         double area = rect.area();
//         double aspect_ratio = static_cast<double>(rect.width) / rect.height;
//         double contour_area = cv::contourArea(contour);
//         double extent = contour_area / area;

//         if (area < 2000 || area > 100000) continue; // Reject small blobs
//         if (aspect_ratio < 0.6 || aspect_ratio > 3.0) continue;
//         if (extent < 0.4) continue;

//         std::vector<cv::Point> hull;
//         cv::convexHull(contour, hull);
//         double hull_area = cv::contourArea(hull);
//         if (hull_area <= 0) continue;
//         double solidity = contour_area / hull_area;
//         if (solidity < 0.7) continue;

//         boxes.push_back(rect);
//     }

//     return boxes;
// }


// File: src/detector.cpp
#include "detector.hpp"
#include <opencv2/imgproc.hpp>
#include <opencv2/bgsegm.hpp>
#include <opencv2/highgui.hpp>

std::vector<cv::Rect> detect_vehicles(const cv::Mat& image,
                                      cv::Mat& gray,
                                      cv::Mat& mask,
                                      cv::Mat& morph) {
    static cv::Ptr<cv::BackgroundSubtractor> bg_subtractor = cv::createBackgroundSubtractorMOG2();

    std::vector<cv::Rect> boxes;

    // Step 1: Convert to grayscale
    cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);

    // Step 2: Apply background subtraction
    bg_subtractor->apply(gray, mask);

    // Step 3: Remove shadows (shadow value = 127)
    cv::threshold(mask, mask, 200, 255, cv::THRESH_BINARY);

    // Step 4: Morphological closing to fill holes and remove noise
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
    cv::morphologyEx(mask, morph, cv::MORPH_CLOSE, kernel);

    // Step 5: Find contours
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(morph, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    for (const auto& contour : contours) {
        cv::Rect rect = cv::boundingRect(contour);
        double area = rect.area();
        double contour_area = cv::contourArea(contour);

        // Filtering
        if (area < 1200 || area > 100000) continue;
        double aspect_ratio = static_cast<double>(rect.width) / rect.height;
        if (aspect_ratio < 0.4 || aspect_ratio > 4.0) continue;
        double extent = contour_area / area;
        if (extent < 0.35) continue;

        std::vector<cv::Point> hull;
        cv::convexHull(contour, hull);
        double hull_area = cv::contourArea(hull);
        if (hull_area <= 0) continue;
        double solidity = contour_area / hull_area;
        if (solidity < 0.6) continue;

        boxes.push_back(rect);
    }

    return boxes;
}
