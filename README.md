# Vehicle Detection and Counting System

This project implements a classical computer vision pipeline to detect and count vehicles in static images using OpenCV in C++. The pipeline avoids the use of machine learning or deep learning models and instead relies on background subtraction, morphological filtering, contour analysis, and geometric heuristics.

---

## Project Structure

```
vehicle_counter/
├── CMakeLists.txt
├── main.cpp
├── include/
│   └── detector.hpp
├── src/
│   └── detector.cpp
├── data/
│   ├── images/              # Input test images
│   └── ground_truth/        # Ground truth annotations in Pascal VOC XML format
├── output/
│   ├── images/              # Output images with detected vehicles and bounding boxes
│   └── results/             # Output text files with bounding box coordinates
├── debug_output/
│   ├── gray/                # Grayscale versions of input images
│   ├── mask/                # Foreground masks after background subtraction
│   └── morph/               # Post-morphology masks
└── README.md                # This file
```

---

## Features

- Classical vehicle detection using:
  - Grayscale conversion
  - Background subtraction (MOG2)
  - Shadow removal
  - Morphological filtering
  - Contour analysis with geometric filtering (area, aspect ratio, solidity, etc.)
- Bounding box generation for each detected vehicle
- Exported results per image (`.txt`)
- Debug output for every processing stage (`gray`, `mask`, `morph`)
- Ready for evaluation against Pascal VOC-formatted ground truth

---

## Techniques Used

- Grayscale conversion
- Background subtraction (MOG2)
- Morphological operations
- Contour extraction
- Shape heuristics (area, aspect ratio, extent, solidity)
- Classical computer vision only — no machine learning

---

## Planned Evaluation Metrics

To be implemented:

- Precision, Recall, F1-score (IoU ≥ 0.5)
- Absolute & Relative vehicle count error
- Per-image result .csv summary
- TP/FP/FN visual overlays

---

## Recent Accomplishments and Improvements

### Enhanced Vehicle Detection Pipeline
- **Improved Background Subtraction**: Enhanced MOG2 background subtraction with better parameter tuning for vehicle detection
- **Advanced Morphological Operations**: Implemented sophisticated morphological filtering including opening, closing, and area-based filtering
- **Robust Contour Analysis**: Added comprehensive geometric heuristics for vehicle validation:
  - Area-based filtering with adaptive thresholds
  - Aspect ratio validation for realistic vehicle proportions
  - Solidity and extent calculations for shape quality assessment
  - Minimum bounding rectangle analysis

### Code Quality Improvements
- **Modular Architecture**: Refactored code into clean, maintainable classes (`Detector` class)
- **Comprehensive Debug Output**: Added multi-stage debug visualization:
  - Grayscale conversion outputs
  - Foreground mask generation
  - Post-morphology processing results
- **Error Handling**: Implemented robust error handling for file operations and image processing

### Performance Optimizations
- **Efficient Image Processing**: Optimized OpenCV operations for better performance
- **Memory Management**: Improved memory usage patterns in image processing pipeline
- **Batch Processing**: Support for processing multiple images with consistent output formatting

### Output and Evaluation Preparation
- **Structured Results**: Generated standardized output format for each processed image
- **Debug Visualization**: Created comprehensive debug outputs for algorithm validation
- **Ground Truth Compatibility**: Prepared output format compatible with Pascal VOC evaluation standards

### Testing and Validation
- **Multi-Image Testing**: Successfully tested pipeline on various traffic scenarios
- **Parameter Tuning**: Optimized detection parameters for different lighting and traffic conditions
- **Quality Assurance**: Implemented validation checks throughout the processing pipeline

---

## Build Instructions

This project uses CMake and requires OpenCV installed on your system.

### Option 1: Native Build
```bash
# Create build directory
mkdir build && cd build

# Configure project
cmake ..

# Build the project
make

# Run the executable
./vehicle_counter
```

### Option 2: Docker (Recommended for easy setup)
```bash
# Build Docker image
docker build -t vehicle-counter .

# Run with Docker Compose
docker-compose up --build

# Or run manually
docker run -v $(pwd)/data:/app/data -v $(pwd)/output:/app/output -v $(pwd)/debug_output:/app/debug_output vehicle-counter
```

**Note**: See `DOCKER_README.md` for detailed Docker instructions.

---

## Input & Output

- Input images: `data/images/*.jpg`
- Ground truth (optional): `data/ground_truth/*.xml`
- Output:
  - Annotated images → `output/images/`
  - Detection results (text) → `output/results/`
  - Debug stages → `debug_output/`

---

## Evaluation (Planned/Next Phase)

The next phase involves building an evaluation module that will:
- Parse Pascal VOC `.xml` files as ground truth
- Match predicted boxes using Intersection over Union (IoU ≥ 0.5)
- Compute metrics:
  - Precision, Recall, F1-Score
  - Absolute and Relative Count Errors

---

## Dependencies

- C++17
- OpenCV (tested on OpenCV 4.x)
- CMake (version ≥ 3.10)

---

## Notes

- No machine learning or deep learning techniques are used
- Vehicle classification (car vs. truck vs. bus) is not currently implemented
- Shadow and noise suppression heuristics may require tuning depending on dataset

---

## ⚙️ Build Instructions for Cloning repo

```bash
# Clone the repository
git clone https://github.com/Aladra8/vehicle_counter.git
cd vehicle_counter

# Build the project
mkdir build && cd build
cmake ..
make
```

---

## Author

Developed by 
Baba Drammeh 
Gulce Sirvanci
University of Padova  
Computer Vision Project

---

## License

This project is licensed for academic and research purposes only.
