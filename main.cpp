#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include "detector.hpp"

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

    fs::create_directories(output_img_dir);
    fs::create_directories(output_txt_dir);
    fs::create_directories(debug_gray_dir);
    fs::create_directories(debug_mask_dir);
    fs::create_directories(debug_morph_dir);

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

        for (const auto& box : car_boxes) {
            cv::rectangle(image, box, cv::Scalar(0, 255, 0), 2);
            cv::putText(image, "car", {box.x, box.y - 5}, cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 1);
        }

        // Save output image
        cv::imwrite(output_img_dir + filename + ".jpg", image);

        // Save output text file
        std::ofstream ofs(output_txt_dir + filename + ".txt");
        if (!ofs) {
            std::cerr << "Failed to write to file: " << output_txt_dir + filename + ".txt" << std::endl;
            continue;
        }
        for (const auto& box : car_boxes) {
            ofs << "car " << box.x << " " << box.y << " " << box.width << " " << box.height << "\n";
        }
        ofs.close();

        // Save debug stages
        if (!gray.empty())  cv::imwrite(debug_gray_dir + filename + ".jpg", gray);
        if (!mask.empty())  cv::imwrite(debug_mask_dir + filename + ".jpg", mask);
        if (!morph.empty()) cv::imwrite(debug_morph_dir + filename + ".jpg", morph);

        std::cout << "[DEBUG] " << filename << " - Found " << car_boxes.size() << " vehicles\n";
    }

    return 0;
}
