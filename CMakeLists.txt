cmake_minimum_required(VERSION 3.20)
project(xinfer-essential VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(FetchContent)

# Feature Flags
option(XINFER_ENABLE_TENSORRT "Enable NVIDIA TensorRT Backend" OFF)
option(XINFER_ENABLE_OPENVINO "Enable Intel OpenVINO Backend" ON)
option(XINFER_ENABLE_RKNN     "Enable Rockchip RKNN Backend"     OFF)
option(XINFER_BUILD_EXAMPLES  "Build example applications"        ON)
option(XINFER_BUILD_TESTS     "Build unit tests and benchmarks"  ON)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

set(CORE_SOURCES
    src/core/memory.cpp
    src/core/tensor.cpp
    src/core/engine.cpp
    src/utils/preproc.cpp
    src/utils/postproc.cpp
)

set(BACKEND_SOURCES "")





# -------------------------------------------------------------------
# 1. TensorRT Setup (NVIDIA)
# -------------------------------------------------------------------
if(XINFER_ENABLE_TENSORRT)
    message(WARNING "CUDA ACTIVATING ${XINFER_ENABLE_TENSORRT}")
    enable_language(CUDA)
    add_compile_definitions(XINFER_ENABLE_TENSORRT)
    find_package(CUDA REQUIRED)
    find_library(TENSORRT_NVINFER nvinfer REQUIRED)
    
    list(APPEND BACKEND_SOURCES src/backends/tensorrt/tensorrt_backend.cpp)
    include_directories(${CUDA_INCLUDE_DIRS})
endif()

# -------------------------------------------------------------------
# 2. OpenVINO Setup (Intel)
# -------------------------------------------------------------------
if(XINFER_ENABLE_OPENVINO)
    add_compile_definitions(XINFER_ENABLE_OPENVINO)
    find_package(OpenVINO QUIET)

    if(OpenVINO_FOUND)
        message(STATUS "[xInfer] Found System OpenVINO: ${OpenVINO_VERSION}")
    else()
        message(WARNING "[xInfer] System OpenVINO not found. Run ./install.sh or install openvino-2024 package.")
    endif()

    list(APPEND BACKEND_SOURCES src/backends/openvino/openvino_backend.cpp)
endif()

# -------------------------------------------------------------------
# 3. RKNN Setup (Rockchip) - With Auto-Fetch Fallback for Headers
# -------------------------------------------------------------------
if(XINFER_ENABLE_RKNN)
    add_compile_definitions(XINFER_ENABLE_RKNN)
    
    find_library(RKNN_RT rknnrt PATHS /usr/local/lib /usr/lib)

    # If headers are missing locally, CMake automatically fetches RKNN C-API from GitHub!
    find_path(RKNN_HEADER_DIR rknn_api.h PATHS /usr/local/include/rknn /usr/include)

    if(NOT RKNN_HEADER_DIR)
        message(STATUS "[xInfer] Local RKNN headers not found. Auto-fetching C-API headers from GitHub...")
        FetchContent_Declare(
            rknpu2_api
            GIT_REPOSITORY https://github.com/rockchip-linux/rknpu2.git
            GIT_TAG        master
            GIT_SHALLOW    TRUE
        )
        FetchContent_MakeAvailable(rknpu2_api)
        include_directories(${rknpu2_api_SOURCE_DIR}/runtime/RK3588/Linux/librknn_api/include)
    else()
        include_directories(${RKNN_HEADER_DIR})
    endif()

    list(APPEND BACKEND_SOURCES src/backends/rknn/rknn_backend.cpp)
endif()

# -------------------------------------------------------------------
# Build Shared Library
# -------------------------------------------------------------------
add_library(xinfer SHARED ${CORE_SOURCES} ${BACKEND_SOURCES})
add_library(xinfer::xinfer ALIAS xinfer)
target_compile_definitions(xinfer PRIVATE XINFER_EXPORTS)

if(XINFER_ENABLE_TENSORRT)
    target_link_libraries(xinfer PRIVATE ${CUDA_LIBRARIES} ${TENSORRT_NVINFER})
endif()

if(XINFER_ENABLE_OPENVINO AND OpenVINO_FOUND)
    target_link_libraries(xinfer PRIVATE openvino::runtime)
endif()

if(XINFER_ENABLE_RKNN AND RKNN_RT)
    target_link_libraries(xinfer PRIVATE ${RKNN_RT})
endif()

if(XINFER_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(XINFER_BUILD_EXAMPLES)
    add_executable(xinfer_demo examples/01_tensorrt_yolo.cpp)
    target_link_libraries(xinfer_demo PRIVATE xinfer::xinfer)
endif()



# ----------------------------------------------------
# INSTALLATION RULES
# ----------------------------------------------------
install(TARGETS xinfer DESTINATION /usr/local/lib)
install(DIRECTORY include/xinfer DESTINATION /usr/local/include)