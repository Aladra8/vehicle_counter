#include "detector.hpp"
#include "config.hpp"
#include <opencv2/imgproc.hpp>
#include <opencv2/bgsegm.hpp>
#include <opencv2/highgui.hpp>

std::vector<cv::Rect> detect_vehicles(const cv::Mat& image,
                                      cv::Mat& gray,
                                      cv::Mat& mask,
                                      cv::Mat& morph) {
    static cv::Ptr<cv::BackgroundSubtractor> bg_subtractor = 
        cv::createBackgroundSubtractorMOG2(
            Config::MOG2_HISTORY,
            Config::MOG2_VAR_THRESHOLD,
            Config::MOG2_DETECT_SHADOWS,
            Config::MOG2_SHADOW_THRESHOLD
        );

    std::vector<cv::Rect> boxes;

    // Step 1: Convert to grayscale
    cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);

    // Step 2: Apply background subtraction
    bg_subtractor->apply(gray, mask);

    // Step 3: Remove shadows (shadow value = 127)
    cv::threshold(mask, mask, Config::MASK_THRESHOLD, Config::MASK_MAX_VALUE, cv::THRESH_BINARY);

    // Step 4: Morphological closing to fill holes and remove noise
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, 
                                              cv::Size(Config::MORPH_KERNEL_SIZE, Config::MORPH_KERNEL_SIZE));
    cv::morphologyEx(mask, morph, Config::MORPH_OPERATION, kernel);

    // Step 5: Find contours
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(morph, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    for (const auto& contour : contours) {
        cv::Rect rect = cv::boundingRect(contour);
        double area = rect.area();
        double contour_area = cv::contourArea(contour);

        // Filtering using config parameters
        if (area < Config::MIN_VEHICLE_AREA || area > Config::MAX_VEHICLE_AREA) continue;
        
        double aspect_ratio = static_cast<double>(rect.width) / rect.height;
        if (aspect_ratio < Config::MIN_ASPECT_RATIO || aspect_ratio > Config::MAX_ASPECT_RATIO) continue;
        
        double extent = contour_area / area;
        if (extent < Config::MIN_EXTENT) continue;

        std::vector<cv::Point> hull;
        cv::convexHull(contour, hull);
        double hull_area = cv::contourArea(hull);
        if (hull_area <= 0) continue;
        
        double solidity = contour_area / hull_area;
        if (solidity < Config::MIN_SOLIDITY) continue;

        boxes.push_back(rect);
    }

    return boxes;
}
