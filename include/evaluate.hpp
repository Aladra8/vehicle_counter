// File: include/evaluate.hpp
#pragma once
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

struct BoundingBox {
    std::string label;
    int x, y, width, height;
};

struct EvaluationResult {
    int tp = 0, fp = 0, fn = 0;
    int n_true = 0, n_pred = 0;
    double precision = 0.0;
    double recall = 0.0;
    double f1 = 0.0;
    double abs_error = 0.0;
    double rel_error = 0.0;
};

std::vector<BoundingBox> parse_ground_truth_xml(const std::string& xml_path);
EvaluationResult evaluate_predictions(const std::vector<BoundingBox>& gt_boxes,
                                      const std::vector<cv::Rect>& pred_boxes);
