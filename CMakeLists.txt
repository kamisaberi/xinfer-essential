cmake_minimum_required(VERSION 3.20)
project(xinfer-essential VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Force hidden visibility by default (only explicit XINFER_API symbols are exported)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

# RPATH configuration so tests and examples can find libxinfer.so at runtime
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# Feature Flags
option(XINFER_ENABLE_TENSORRT "Enable NVIDIA TensorRT Backend" ON)
option(XINFER_ENABLE_OPENVINO "Enable Intel OpenVINO Backend" OFF)
option(XINFER_ENABLE_RKNN     "Enable Rockchip RKNN Backend"     OFF)
option(XINFER_BUILD_EXAMPLES  "Build example applications"        ON)
option(XINFER_BUILD_TESTS     "Build unit tests and benchmarks"  ON)

# Include Directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Core Sources
set(CORE_SOURCES
    src/core/memory.cpp
    src/core/tensor.cpp
    src/core/engine.cpp
    src/utils/preproc.cpp
    src/utils/postproc.cpp
)

set(BACKEND_SOURCES "")

# TensorRT Configuration
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

# OpenVINO Configuration
if(XINFER_ENABLE_OPENVINO)
    add_compile_definitions(XINFER_ENABLE_OPENVINO)
    find_package(OpenVINO REQUIRED)
    list(APPEND BACKEND_SOURCES
        src/backends/openvino/openvino_backend.cpp
    )
endif()

# RKNN Configuration
if(XINFER_ENABLE_RKNN)
    add_compile_definitions(XINFER_ENABLE_RKNN)
    find_library(RKNN_RT rknnrt REQUIRED)
    list(APPEND BACKEND_SOURCES
        src/backends/rknn/rknn_backend.cpp
    )
endif()

# ----------------------------------------------------
# BUILD SHARED LIBRARY (libxinfer.so / xinfer.dll)
# ----------------------------------------------------
add_library(xinfer SHARED ${CORE_SOURCES} ${BACKEND_SOURCES})
add_library(xinfer::xinfer ALIAS xinfer)

# Define XINFER_EXPORTS when building the shared library
target_compile_definitions(xinfer PRIVATE XINFER_EXPORTS)

set_target_properties(xinfer PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    OUTPUT_NAME "xinfer"
)

# Link Dependencies
if(XINFER_ENABLE_TENSORRT)
    target_link_libraries(xinfer PRIVATE ${CUDA_LIBRARIES} ${TENSORRT_NVINFER})
endif()

if(XINFER_ENABLE_OPENVINO)
    target_link_libraries(xinfer PRIVATE openvino::runtime)
endif()

if(XINFER_ENABLE_RKNN)
    target_link_libraries(xinfer PRIVATE ${RKNN_RT})
endif()

# Include Subdirectories
if(XINFER_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(XINFER_BUILD_EXAMPLES)
    add_executable(xinfer_demo examples/01_tensorrt_yolo.cpp)
    target_link_libraries(xinfer_demo PRIVATE xinfer::xinfer)
endif()