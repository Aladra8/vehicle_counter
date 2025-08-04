# File: CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(vehicle_counter)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)

include_directories(include)

add_executable(vehicle_counter
    main.cpp
    src/detector.cpp
)

# Link filesystem explicitly (required for GCC < 9)
target_link_libraries(vehicle_counter ${OpenCV_LIBS} stdc++fs)

# cmake_minimum_required(VERSION 3.10)
# project(VehicleCounter)

# # Set C++ standard
# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

# # Find OpenCV
# find_package(OpenCV REQUIRED)
# include_directories(${OpenCV_INCLUDE_DIRS})

# # Include project headers
# include_directories(${PROJECT_SOURCE_DIR}/include)

# # Source files
# set(SOURCES
#     src/detector.cpp
#     main.cpp
# )

# # Create executable
# add_executable(vehicle_counter ${SOURCES})

# # Link OpenCV libraries
# target_link_libraries(vehicle_counter ${OpenCV_LIBS})