// File: src/evaluate.cpp
#include "evaluate.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <filesystem>
#include <opencv2/imgproc.hpp>

namespace fs = std::filesystem;

std::vector<Annotation> parseVOCXML(const std::string& xml_path) {
    std::vector<Annotation> annotations;
    
    std::ifstream file(xml_path);
    if (!file.is_open()) {
        std::cerr << "Failed to open XML file: " << xml_path << std::endl;
        return annotations;
    }
    
    std::string line;
    std::string filename;
    
    while (std::getline(file, line)) {
        // Extract filename
        if (line.find("<filename>") != std::string::npos) {
            size_t start = line.find("<filename>") + 10;
            size_t end = line.find("</filename>");
            if (end != std::string::npos) {
                filename = line.substr(start, end - start);
            }
        }
        
        // Extract object annotations
        if (line.find("<object>") != std::string::npos) {
            Annotation ann;
            ann.filename = filename;
            
            // Read until </object>
            while (std::getline(file, line) && line.find("</object>") == std::string::npos) {
                // Extract class name
                if (line.find("<name>") != std::string::npos) {
                    size_t start = line.find("<name>") + 6;
                    size_t end = line.find("</name>");
                    if (end != std::string::npos) {
                        ann.class_name = line.substr(start, end - start);
                    }
                }
                
                // Extract bounding box
                if (line.find("<bndbox>") != std::string::npos) {
                    int xmin = 0, ymin = 0, xmax = 0, ymax = 0;
                    
                    while (std::getline(file, line) && line.find("</bndbox>") == std::string::npos) {
                        if (line.find("<xmin>") != std::string::npos) {
                            xmin = std::stoi(line.substr(line.find("<xmin>") + 6, line.find("</xmin>") - line.find("<xmin>") - 6));
                        }
                        if (line.find("<ymin>") != std::string::npos) {
                            ymin = std::stoi(line.substr(line.find("<ymin>") + 6, line.find("</ymin>") - line.find("<ymin>") - 6));
                        }
                        if (line.find("<xmax>") != std::string::npos) {
                            xmax = std::stoi(line.substr(line.find("<xmax>") + 6, line.find("</xmax>") - line.find("<xmax>") - 6));
                        }
                        if (line.find("<ymax>") != std::string::npos) {
                            ymax = std::stoi(line.substr(line.find("<ymax>") + 6, line.find("</ymax>") - line.find("<ymax>") - 6));
                        }
                    }
                    
                    ann.bbox = cv::Rect(xmin, ymin, xmax - xmin, ymax - ymin);
                }
            }
            
            if (!ann.class_name.empty() && ann.bbox.area() > 0) {
                annotations.push_back(ann);
            }
        }
    }
    
    return annotations;
}

std::vector<Detection> parseDetectionResults(const std::string& txt_path) {
    std::vector<Detection> detections;
    
    std::ifstream file(txt_path);
    if (!file.is_open()) {
        std::cerr << "Failed to open detection file: " << txt_path << std::endl;
        return detections;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        Detection det;
        
        if (iss >> det.class_name >> det.bbox.x >> det.bbox.y >> det.bbox.width >> det.bbox.height) {
            det.confidence = 1.0; // Default confidence for classical CV approach
            detections.push_back(det);
        }
    }
    
    return detections;
}

double calculateIoU(const cv::Rect& rect1, const cv::Rect& rect2) {
    // Calculate intersection
    int x1 = std::max(rect1.x, rect2.x);
    int y1 = std::max(rect1.y, rect2.y);
    int x2 = std::min(rect1.x + rect1.width, rect2.x + rect2.width);
    int y2 = std::min(rect1.y + rect1.height, rect2.y + rect2.height);
    
    if (x2 <= x1 || y2 <= y1) {
        return 0.0; // No intersection
    }
    
    int intersection_area = (x2 - x1) * (y2 - y1);
    int union_area = rect1.area() + rect2.area() - intersection_area;
    
    return static_cast<double>(intersection_area) / union_area;
}

EvaluationMetrics evaluateDetections(
    const std::vector<Detection>& detections,
    const std::vector<Annotation>& ground_truth,
    double iou_threshold) {
    
    EvaluationMetrics metrics = {0.0, 0.0, 0.0, 0, 0, 0, 0.0, 0.0};
    
    if (ground_truth.empty() && detections.empty()) {
        metrics.precision = 1.0;
        metrics.recall = 1.0;
        metrics.f1_score = 1.0;
        return metrics;
    }
    
    if (ground_truth.empty()) {
        metrics.precision = 0.0;
        metrics.recall = 1.0;
        metrics.f1_score = 0.0;
        metrics.false_positives = detections.size();
        return metrics;
    }
    
    if (detections.empty()) {
        metrics.precision = 1.0;
        metrics.recall = 0.0;
        metrics.f1_score = 0.0;
        metrics.false_negatives = ground_truth.size();
        return metrics;
    }
    
    std::vector<bool> gt_matched(ground_truth.size(), false);
    std::vector<bool> det_matched(detections.size(), false);
    
    // Match detections to ground truth
    for (size_t i = 0; i < detections.size(); ++i) {
        double best_iou = 0.0;
        int best_gt_idx = -1;
        
        for (size_t j = 0; j < ground_truth.size(); ++j) {
            if (!gt_matched[j] && detections[i].class_name == ground_truth[j].class_name) {
                double iou = calculateIoU(detections[i].bbox, ground_truth[j].bbox);
                if (iou > best_iou && iou >= iou_threshold) {
                    best_iou = iou;
                    best_gt_idx = j;
                }
            }
        }
        
        if (best_gt_idx >= 0) {
            gt_matched[best_gt_idx] = true;
            det_matched[i] = true;
            metrics.true_positives++;
        } else {
            metrics.false_positives++;
        }
    }
    
    // Count unmatched ground truth as false negatives
    for (size_t i = 0; i < ground_truth.size(); ++i) {
        if (!gt_matched[i]) {
            metrics.false_negatives++;
        }
    }
    
    // Calculate metrics
    if (metrics.true_positives + metrics.false_positives > 0) {
        metrics.precision = static_cast<double>(metrics.true_positives) / 
                          (metrics.true_positives + metrics.false_positives);
    }
    
    if (metrics.true_positives + metrics.false_negatives > 0) {
        metrics.recall = static_cast<double>(metrics.true_positives) / 
                        (metrics.true_positives + metrics.false_negatives);
    }
    
    if (metrics.precision + metrics.recall > 0) {
        metrics.f1_score = 2.0 * metrics.precision * metrics.recall / 
                          (metrics.precision + metrics.recall);
    }
    
    // Calculate count errors
    int gt_count = ground_truth.size();
    int pred_count = detections.size();
    metrics.count_error = std::abs(gt_count - pred_count);
    metrics.relative_count_error = gt_count > 0 ? metrics.count_error / gt_count : 0.0;
    
    return metrics;
}

void generateEvaluationReport(
    const std::string& output_dir,
    const std::map<std::string, EvaluationMetrics>& per_image_metrics,
    const EvaluationMetrics& overall_metrics) {
    
    fs::create_directories(output_dir);
    
    // Generate per-image CSV report
    std::ofstream csv_file(output_dir + "/per_image_metrics.csv");
    csv_file << "Image,Precision,Recall,F1_Score,TP,FP,FN,Count_Error,Relative_Count_Error\n";
    
    for (const auto& [filename, metrics] : per_image_metrics) {
        csv_file << filename << ","
                << metrics.precision << ","
                << metrics.recall << ","
                << metrics.f1_score << ","
                << metrics.true_positives << ","
                << metrics.false_positives << ","
                << metrics.false_negatives << ","
                << metrics.count_error << ","
                << metrics.relative_count_error << "\n";
    }
    csv_file.close();
    
    // Generate overall summary
    std::ofstream summary_file(output_dir + "/overall_summary.txt");
    summary_file << "=== OVERALL EVALUATION SUMMARY ===\n\n";
    summary_file << "Precision: " << overall_metrics.precision << "\n";
    summary_file << "Recall: " << overall_metrics.recall << "\n";
    summary_file << "F1-Score: " << overall_metrics.f1_score << "\n";
    summary_file << "True Positives: " << overall_metrics.true_positives << "\n";
    summary_file << "False Positives: " << overall_metrics.false_positives << "\n";
    summary_file << "False Negatives: " << overall_metrics.false_negatives << "\n";
    summary_file << "Absolute Count Error: " << overall_metrics.count_error << "\n";
    summary_file << "Relative Count Error: " << overall_metrics.relative_count_error << "\n";
    summary_file.close();
}

void visualizeEvaluationResults(
    const cv::Mat& image,
    const std::vector<Detection>& detections,
    const std::vector<Annotation>& ground_truth,
    const std::string& output_path) {
    
    cv::Mat result = image.clone();
    
    // Draw ground truth boxes in blue
    for (const auto& gt : ground_truth) {
        cv::rectangle(result, gt.bbox, cv::Scalar(255, 0, 0), 2);
        cv::putText(result, "GT: " + gt.class_name, 
                   cv::Point(gt.bbox.x, gt.bbox.y - 5),
                   cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 0, 0), 1);
    }
    
    // Draw detection boxes in green
    for (const auto& det : detections) {
        cv::rectangle(result, det.bbox, cv::Scalar(0, 255, 0), 2);
        cv::putText(result, "DET: " + det.class_name, 
                   cv::Point(det.bbox.x, det.bbox.y - 5),
                   cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 1);
    }
    
    cv::imwrite(output_path, result);
}
