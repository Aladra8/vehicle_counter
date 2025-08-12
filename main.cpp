#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include "detector.hpp"
#include "evaluate.hpp"
#include "config.hpp"

#if defined(__cpp_lib_filesystem)
    #include <filesystem>
    namespace fs = std::filesystem;
#elif __has_include(<experimental/filesystem>)
    #include <experimental/filesystem>
    namespace fs = std::experimental::filesystem;
#else
    #error "No filesystem support"
#endif

int main() {
    std::string image_dir = "data/images/";
    std::string output_img_dir = "output/images/";
    std::string output_txt_dir = "output/results/";
    std::string debug_gray_dir = "debug_output/gray/";
    std::string debug_mask_dir = "debug_output/mask/";
    std::string debug_morph_dir = "debug_output/morph/";
    std::string evaluation_dir = "output/evaluation/";

    fs::create_directories(output_img_dir);
    fs::create_directories(output_txt_dir);
    fs::create_directories(debug_gray_dir);
    fs::create_directories(debug_mask_dir);
    fs::create_directories(debug_morph_dir);
    fs::create_directories(evaluation_dir);

    // Evaluation data structures
    std::map<std::string, EvaluationMetrics> per_image_metrics;
    EvaluationMetrics overall_metrics = {0.0, 0.0, 0.0, 0, 0, 0, 0.0, 0.0};
    int total_gt_vehicles = 0;
    int total_detected_vehicles = 0;

    for (const auto& entry : fs::directory_iterator(image_dir)) {
        std::string img_path = entry.path().string();
        std::string filename = entry.path().stem().string();

        cv::Mat image = cv::imread(img_path);
        if (image.empty()) {
            std::cerr << "Failed to load image: " << img_path << std::endl;
            continue;
        }

        // Pass-by-reference debug mats
        cv::Mat gray, mask, morph;
        std::vector<cv::Rect> car_boxes = detect_vehicles(image.clone(), gray, mask, morph);

        // Convert detections to Detection format
        std::vector<Detection> detections;
        for (const auto& box : car_boxes) {
            Detection det;
            det.class_name = "car";
            det.bbox = box;
            det.confidence = 1.0;
            detections.push_back(det);
        }

        // Load ground truth if available
        std::vector<Annotation> ground_truth;
        std::string gt_path = "data/ground_truth/" + filename + ".xml";
        if (fs::exists(gt_path)) {
            ground_truth = parseVOCXML(gt_path);
            total_gt_vehicles += ground_truth.size();
        }

        // Evaluate detections
        if (!ground_truth.empty()) {
            EvaluationMetrics metrics = evaluateDetections(detections, ground_truth, 0.5);
            per_image_metrics[filename] = metrics;
            
            // Accumulate overall metrics
            overall_metrics.true_positives += metrics.true_positives;
            overall_metrics.false_positives += metrics.false_positives;
            overall_metrics.false_negatives += metrics.false_negatives;
        }

        total_detected_vehicles += detections.size();

        // Draw detection boxes on image
        for (const auto& det : detections) {
            cv::rectangle(image, det.bbox, Config::DETECTION_COLOR, Config::BBOX_THICKNESS);
            cv::putText(image, det.class_name, {det.bbox.x, det.bbox.y - 5}, 
                       cv::FONT_HERSHEY_SIMPLEX, Config::FONT_SCALE, Config::TEXT_COLOR, Config::FONT_THICKNESS);
        }

        // Save output image
        cv::imwrite(output_img_dir + filename + ".jpg", image);

        // Save output text file
        std::ofstream ofs(output_txt_dir + filename + ".txt");
        if (!ofs) {
            std::cerr << "Failed to write to file: " << output_txt_dir + filename + ".txt" << std::endl;
            continue;
        }
        for (const auto& det : detections) {
            ofs << det.class_name << " " << det.bbox.x << " " << det.bbox.y 
                << " " << det.bbox.width << " " << det.bbox.height << "\n";
        }
        ofs.close();

        // Save debug stages
        if (!gray.empty())  cv::imwrite(debug_gray_dir + filename + ".jpg", gray);
        if (!mask.empty())  cv::imwrite(debug_mask_dir + filename + ".jpg", mask);
        if (!morph.empty()) cv::imwrite(debug_morph_dir + filename + ".jpg", morph);

        // Generate evaluation visualization if ground truth exists
        if (!ground_truth.empty()) {
            std::string eval_img_path = evaluation_dir + filename + "_evaluation.jpg";
            visualizeEvaluationResults(image, detections, ground_truth, eval_img_path);
        }

        std::cout << "[DEBUG] " << filename << " - Found " << detections.size() 
                  << " vehicles, GT: " << ground_truth.size() << "\n";
    }

    // Calculate overall metrics
    if (overall_metrics.true_positives + overall_metrics.false_positives > 0) {
        overall_metrics.precision = static_cast<double>(overall_metrics.true_positives) / 
                                  (overall_metrics.true_positives + overall_metrics.false_positives);
    }
    
    if (overall_metrics.true_positives + overall_metrics.false_negatives > 0) {
        overall_metrics.recall = static_cast<double>(overall_metrics.true_positives) / 
                                (overall_metrics.true_positives + overall_metrics.false_negatives);
    }
    
    if (overall_metrics.precision + overall_metrics.recall > 0) {
        overall_metrics.f1_score = 2.0 * overall_metrics.precision * overall_metrics.recall / 
                                  (overall_metrics.precision + overall_metrics.recall);
    }
    
    overall_metrics.count_error = std::abs(total_gt_vehicles - total_detected_vehicles);
    overall_metrics.relative_count_error = total_gt_vehicles > 0 ? 
                                         overall_metrics.count_error / total_gt_vehicles : 0.0;

    // Generate evaluation report
    generateEvaluationReport(evaluation_dir, per_image_metrics, overall_metrics);

    std::cout << "\n=== EVALUATION SUMMARY ===" << std::endl;
    std::cout << "Overall Precision: " << overall_metrics.precision << std::endl;
    std::cout << "Overall Recall: " << overall_metrics.recall << std::endl;
    std::cout << "Overall F1-Score: " << overall_metrics.f1_score << std::endl;
    std::cout << "Total Ground Truth Vehicles: " << total_gt_vehicles << std::endl;
    std::cout << "Total Detected Vehicles: " << total_detected_vehicles << std::endl;
    std::cout << "Absolute Count Error: " << overall_metrics.count_error << std::endl;
    std::cout << "Relative Count Error: " << overall_metrics.relative_count_error << std::endl;

    return 0;
}
