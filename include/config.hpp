#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <opencv2/opencv.hpp>

namespace Config {
    // Background subtraction parameters
    constexpr int MOG2_HISTORY = 500;
    constexpr int MOG2_VAR_THRESHOLD = 16;
    constexpr bool MOG2_DETECT_SHADOWS = true;
    constexpr double MOG2_SHADOW_THRESHOLD = 0.5;
    
    // Threshold parameters
    constexpr int MASK_THRESHOLD = 200;
    constexpr int MASK_MAX_VALUE = 255;
    
    // Morphological parameters
    constexpr int MORPH_KERNEL_SIZE = 5;
    constexpr int MORPH_OPERATION = cv::MORPH_CLOSE;
    
    // Vehicle detection parameters
    constexpr int MIN_VEHICLE_AREA = 1200;
    constexpr int MAX_VEHICLE_AREA = 100000;
    constexpr double MIN_ASPECT_RATIO = 0.4;
    constexpr double MAX_ASPECT_RATIO = 4.0;
    constexpr double MIN_EXTENT = 0.35;
    constexpr double MIN_SOLIDITY = 0.6;
    
    // Evaluation parameters
    constexpr double IOU_THRESHOLD = 0.5;
    
    // Output parameters
    constexpr int BBOX_THICKNESS = 2;
    constexpr double FONT_SCALE = 0.5;
    constexpr int FONT_THICKNESS = 1;
    
    // Colors (BGR format)
    const cv::Scalar DETECTION_COLOR(0, 255, 0);    // Green
    const cv::Scalar GROUND_TRUTH_COLOR(255, 0, 0); // Blue
    const cv::Scalar TEXT_COLOR(0, 255, 0);         // Green
}

#endif // CONFIG_HPP
