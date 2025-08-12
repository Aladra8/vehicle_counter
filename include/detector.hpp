#ifndef DETECTOR_HPP
#define DETECTOR_HPP

#include <opencv2/opencv.hpp>
#include <vector>

std::vector<cv::Rect> detect_vehicles(const cv::Mat& image, cv::Mat& gray, cv::Mat& mask, cv::Mat& morph);

#endif // DETECTOR_HPP
