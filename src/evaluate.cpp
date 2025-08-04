// File: src/evaluate.cpp
#include "evaluate.hpp"
#include <tinyxml2.h>
#include <cmath>
#include <set>

using namespace tinyxml2;

std::vector<BoundingBox> parse_ground_truth_xml(const std::string& xml_path) {
    std::vector<BoundingBox> boxes;
    XMLDocument doc;
    if (doc.LoadFile(xml_path.c_str()) != XML_SUCCESS) return boxes;

    XMLElement* root = doc.FirstChildElement("annotation");
    for (XMLElement* obj = root->FirstChildElement("object"); obj; obj = obj->NextSiblingElement("object")) {
        const char* label = obj->FirstChildElement("name")->GetText();
        XMLElement* box = obj->FirstChildElement("bndbox");
        if (!label || !box) continue;

        BoundingBox b;
        b.label = label;
        box->FirstChildElement("xmin")->QueryIntText(&b.x);
        box->FirstChildElement("ymin")->QueryIntText(&b.y);
        int xmax, ymax;
        box->FirstChildElement("xmax")->QueryIntText(&xmax);
        box->FirstChildElement("ymax")->QueryIntText(&ymax);
        b.width = xmax - b.x;
        b.height = ymax - b.y;
        boxes.push_back(b);
    }
    return boxes;
}

double compute_iou(const BoundingBox& a, const cv::Rect& b) {
    cv::Rect ra(a.x, a.y, a.width, a.height);
    int x1 = std::max(ra.x, b.x);
    int y1 = std::max(ra.y, b.y);
    int x2 = std::min(ra.x + ra.width, b.x + b.width);
    int y2 = std::min(ra.y + ra.height, b.y + b.height);

    int interArea = std::max(0, x2 - x1) * std::max(0, y2 - y1);
    int unionArea = ra.area() + b.area() - interArea;
    return unionArea > 0 ? static_cast<double>(interArea) / unionArea : 0.0;
}

EvaluationResult evaluate_predictions(const std::vector<BoundingBox>& gt_boxes,
                                      const std::vector<cv::Rect>& pred_boxes) {
    EvaluationResult result;
    result.n_true = gt_boxes.size();
    result.n_pred = pred_boxes.size();
    std::set<int> matched_preds;

    for (const auto& gt : gt_boxes) {
        bool matched = false;
        for (size_t j = 0; j < pred_boxes.size(); ++j) {
            if (matched_preds.count(j)) continue;
            double iou = compute_iou(gt, pred_boxes[j]);
            if (iou >= 0.5) {
                matched_preds.insert(j);
                matched = true;
                break;
            }
        }
        if (matched) result.tp++;
        else result.fn++;
    }

    result.fp = result.n_pred - result.tp;

    result.precision = result.n_pred > 0 ? static_cast<double>(result.tp) / result.n_pred : 0;
    result.recall = result.n_true > 0 ? static_cast<double>(result.tp) / result.n_true : 0;
    result.f1 = (result.precision + result.recall) > 0
              ? 2 * result.precision * result.recall / (result.precision + result.recall)
              : 0;

    result.abs_error = std::abs(result.n_pred - result.n_true);
    result.rel_error = result.n_true > 0 ? result.abs_error / static_cast<double>(result.n_true) : 0;

    return result;
}
