cmake_minimum_required(VERSION 3.10)
project(vehicle_counter)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)

include_directories(include)

add_executable(vehicle_counter
    main.cpp
    src/detector.cpp
    src/evaluate.cpp
)

target_link_libraries(vehicle_counter ${OpenCV_LIBS})