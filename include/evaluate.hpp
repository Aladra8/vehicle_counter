#ifndef EVALUATE_HPP
#define EVALUATE_HPP

#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include <map>

// Ground truth annotation structure
struct Annotation {
    std::string filename;
    std::string class_name;
    cv::Rect bbox;
};

// Detection result structure
struct Detection {
    std::string class_name;
    cv::Rect bbox;
    double confidence;
};

// Evaluation metrics structure
struct EvaluationMetrics {
    double precision;
    double recall;
    double f1_score;
    int true_positives;
    int false_positives;
    int false_negatives;
    double count_error;
    double relative_count_error;
};

// Parse Pascal VOC XML file
std::vector<Annotation> parseVOCXML(const std::string& xml_path);

// Parse detection results from text file
std::vector<Detection> parseDetectionResults(const std::string& txt_path);

// Calculate Intersection over Union (IoU)
double calculateIoU(const cv::Rect& rect1, const cv::Rect& rect2);

// Evaluate detections against ground truth
EvaluationMetrics evaluateDetections(
    const std::vector<Detection>& detections,
    const std::vector<Annotation>& ground_truth,
    double iou_threshold = 0.5
);

// Generate evaluation report
void generateEvaluationReport(
    const std::string& output_dir,
    const std::map<std::string, EvaluationMetrics>& per_image_metrics,
    const EvaluationMetrics& overall_metrics
);

// Visualize evaluation results
void visualizeEvaluationResults(
    const cv::Mat& image,
    const std::vector<Detection>& detections,
    const std::vector<Annotation>& ground_truth,
    const std::string& output_path
);

#endif // EVALUATE_HPP
