cmake_minimum_required(VERSION 3.20)
project(xinfer-essential VERSION 1.0.0 LANGUAGES CXX C Coded)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Feature Flags
option(XINFER_ENABLE_TENSORRT "Enable NVIDIA TensorRT Backend" ON)
option(XINFER_ENABLE_OPENVINO "Enable Intel OpenVINO Backend" OFF)
option(XINFER_ENABLE_RKNN     "Enable Rockchip RKNN Backend"     OFF)
option(XINFER_BUILD_EXAMPLES  "Build example applications"        ON)

# Include Directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Collect Core Sources
set(CORE_SOURCES
    src/core/memory.cpp
    src/core/tensor.cpp
    src/core/engine.cpp
)

set(BACKEND_SOURCES "")

# TensorRT Setup
if(XINFER_ENABLE_TENSORRT)
    enable_language(CUDA)
    add_compile_definitions(XINFER_ENABLE_TENSORRT)
    find_package(CUDA REQUIRED)
    find_library(TENSORRT_NVINFER nvinfer REQUIRED)
    
    list(APPEND BACKEND_SOURCES
        src/backends/tensorrt/tensorrt_backend.cpp
    )
    include_directories(${CUDA_INCLUDE_DIRS})
endif()

# OpenVINO Setup
if(XINFER_ENABLE_OPENVINO)
    add_compile_definitions(XINFER_ENABLE_OPENVINO)
    find_package(OpenVINO REQUIRED)
    list(APPEND BACKEND_SOURCES
        src/backends/openvino/openvino_backend.cpp
    )
endif()

# Create Static Library
add_library(xinfer_core STATIC ${CORE_SOURCES} ${BACKEND_SOURCES})

if(XINFER_ENABLE_TENSORRT)
    target_link_libraries(xinfer_core PRIVATE ${CUDA_LIBRARIES} ${TENSORRT_NVINFER})
endif()

if(XINFER_ENABLE_OPENVINO)
    target_link_libraries(xinfer_core PRIVATE openvino::runtime)
endif()

# Examples
if(XINFER_BUILD_EXAMPLES)
    add_executable(xinfer_demo examples/main.cpp)
    target_link_libraries(xinfer_demo PRIVATE xinfer_core)
endif()