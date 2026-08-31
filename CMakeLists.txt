cmake_minimum_required(VERSION 3.20)
project(xinfer-essential VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Enforce hidden symbol visibility presets for Shared Library export
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

# RPATH configuration: Ensures binaries always use local libxinfer.so during build & testing
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

include(FetchContent)

# -------------------------------------------------------------------
# ALL 15 HARDWARE BACKEND TOGGLES
# -------------------------------------------------------------------
option(XINFER_ENABLE_TENSORRT   "Enable NVIDIA TensorRT Backend"          OFF)
option(XINFER_ENABLE_OPENVINO   "Enable Intel OpenVINO Backend"          ON)
option(XINFER_ENABLE_RKNN       "Enable Rockchip RKNN Backend"              OFF)
option(XINFER_ENABLE_VITIS_AI   "Enable AMD/Xilinx Vitis AI Backend"        OFF)
option(XINFER_ENABLE_QNN        "Enable Qualcomm QNN Backend"               OFF)
option(XINFER_ENABLE_COREML     "Enable Apple CoreML Backend"               OFF)
option(XINFER_ENABLE_RYZEN_AI   "Enable AMD Ryzen AI Backend"             OFF)
option(XINFER_ENABLE_NEUROPILOT "Enable MediaTek NeuroPilot Backend"     OFF)
option(XINFER_ENABLE_HAILO      "Enable Hailo HailoRT Backend"              OFF)
option(XINFER_ENABLE_CVFLOW     "Enable Ambarella CVFlow Backend"        OFF)
option(XINFER_ENABLE_ENN        "Enable Samsung ENN Backend"              OFF)
option(XINFER_ENABLE_EDGE_TPU   "Enable Google Coral Edge TPU Backend"     OFF)
option(XINFER_ENABLE_FPGA_AI    "Enable Intel FPGA AI Suite Backend"     OFF)
option(XINFER_ENABLE_VECTORBLOX "Enable Microchip VectorBlox Backend"     OFF)
option(XINFER_ENABLE_SENSAI     "Enable Lattice sensAI Backend"           OFF)

option(XINFER_BUILD_TESTS       "Build unit tests and benchmarks"           OFF)
option(XINFER_BUILD_EXAMPLES    "Build application examples"                OFF)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

set(CORE_SOURCES
    src/core/memory.cpp
    src/core/tensor.cpp
    src/core/engine.cpp
    src/core/hub.cpp
    src/utils/preproc.cpp
    src/utils/postproc.cpp
    src/core/plugin_manager.cpp   # <--- ADDED PLUGIN MANAGER

)

set(BACKEND_SOURCES "")

# -------------------------------------------------------------------
# 1. TensorRT Setup (NVIDIA)
# -------------------------------------------------------------------
if(XINFER_ENABLE_TENSORRT)
    message(STATUS "[xInfer] Enabling NVIDIA TensorRT Backend")
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
# 4. AMD / Xilinx Vitis AI Setup
# -------------------------------------------------------------------
if(XINFER_ENABLE_VITIS_AI)
    add_compile_definitions(XINFER_ENABLE_VITIS_AI)
    list(APPEND BACKEND_SOURCES src/backends/vitis_ai/vitis_backend.cpp)
endif()

# -------------------------------------------------------------------
# 5. Qualcomm QNN Setup
# -------------------------------------------------------------------
if(XINFER_ENABLE_QNN)
    add_compile_definitions(XINFER_ENABLE_QNN)
    list(APPEND BACKEND_SOURCES src/backends/qnn/qnn_backend.cpp)
endif()

# -------------------------------------------------------------------
# 6. Apple CoreML Setup
# -------------------------------------------------------------------
if(XINFER_ENABLE_COREML)
    add_compile_definitions(XINFER_ENABLE_COREML)
    if(APPLE)
        enable_language(OBJCXX)
        list(APPEND BACKEND_SOURCES src/backends/coreml/coreml_backend.mm)
        find_library(COREML_FRAMEWORK CoreML REQUIRED)
        find_library(METAL_FRAMEWORK Metal REQUIRED)
        find_library(FOUNDATION_FRAMEWORK Foundation REQUIRED)
    else()
        list(APPEND BACKEND_SOURCES src/backends/coreml/coreml_backend.cpp)
    endif()
endif()

# -------------------------------------------------------------------
# 7. AMD Ryzen AI Setup
# -------------------------------------------------------------------
if(XINFER_ENABLE_RYZEN_AI)
    add_compile_definitions(XINFER_ENABLE_RYZEN_AI)
    list(APPEND BACKEND_SOURCES src/backends/ryzen_ai/ryzen_backend.cpp)
endif()

# -------------------------------------------------------------------
# 8. MediaTek NeuroPilot Setup
# -------------------------------------------------------------------
if(XINFER_ENABLE_NEUROPILOT)
    add_compile_definitions(XINFER_ENABLE_NEUROPILOT)
    list(APPEND BACKEND_SOURCES src/backends/neuropilot/neuropilot_backend.cpp)
endif()

# -------------------------------------------------------------------
# 9. Hailo HailoRT Setup
# -------------------------------------------------------------------
if(XINFER_ENABLE_HAILO)
    add_compile_definitions(XINFER_ENABLE_HAILO)
    list(APPEND BACKEND_SOURCES src/backends/hailo/hailo_backend.cpp)
endif()

# -------------------------------------------------------------------
# 10. Ambarella CVFlow Setup
# -------------------------------------------------------------------
if(XINFER_ENABLE_CVFLOW)
    add_compile_definitions(XINFER_ENABLE_CVFLOW)
    list(APPEND BACKEND_SOURCES src/backends/cvflow/cvflow_backend.cpp)
endif()

# -------------------------------------------------------------------
# 11. Samsung ENN Setup
# -------------------------------------------------------------------
if(XINFER_ENABLE_ENN)
    add_compile_definitions(XINFER_ENABLE_ENN)
    list(APPEND BACKEND_SOURCES src/backends/enn/enn_backend.cpp)
endif()

# -------------------------------------------------------------------
# 12. Google Coral Edge TPU Setup
# -------------------------------------------------------------------
if(XINFER_ENABLE_EDGE_TPU)
    add_compile_definitions(XINFER_ENABLE_EDGE_TPU)
    list(APPEND BACKEND_SOURCES src/backends/edge_tpu/edgetpu_backend.cpp)
endif()

# -------------------------------------------------------------------
# 13. Intel FPGA AI Suite Setup
# -------------------------------------------------------------------
if(XINFER_ENABLE_FPGA_AI)
    add_compile_definitions(XINFER_ENABLE_FPGA_AI)
    list(APPEND BACKEND_SOURCES src/backends/fpga_ai/fpga_backend.cpp)
endif()

# -------------------------------------------------------------------
# 14. Microchip VectorBlox Setup
# -------------------------------------------------------------------
if(XINFER_ENABLE_VECTORBLOX)
    add_compile_definitions(XINFER_ENABLE_VECTORBLOX)
    list(APPEND BACKEND_SOURCES src/backends/vectorblox/vectorblox_backend.cpp)
endif()

# -------------------------------------------------------------------
# 15. Lattice sensAI Setup
# -------------------------------------------------------------------
if(XINFER_ENABLE_SENSAI)
    add_compile_definitions(XINFER_ENABLE_SENSAI)
    list(APPEND BACKEND_SOURCES src/backends/sensai/sensai_backend.cpp)
endif()

# -------------------------------------------------------------------
# Build Shared Library (libxinfer.so)
# -------------------------------------------------------------------
add_library(xinfer SHARED ${CORE_SOURCES} ${BACKEND_SOURCES})
add_library(xinfer::xinfer ALIAS xinfer)
target_compile_definitions(xinfer PRIVATE XINFER_EXPORTS)

set_target_properties(xinfer PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    OUTPUT_NAME "xinfer"
)

# -------------------------------------------------------------------
# Link Hardware Vendor Libraries
# -------------------------------------------------------------------
if(XINFER_ENABLE_TENSORRT)
    target_link_libraries(xinfer PRIVATE ${CUDA_LIBRARIES} ${TENSORRT_NVINFER})
endif()

if(XINFER_ENABLE_OPENVINO AND OpenVINO_FOUND)
    target_link_libraries(xinfer PRIVATE openvino::runtime)
endif()

if(XINFER_ENABLE_RKNN AND RKNN_RT)
    target_link_libraries(xinfer PRIVATE ${RKNN_RT})
endif()

if(XINFER_ENABLE_COREML AND APPLE)
    target_link_libraries(xinfer PRIVATE ${COREML_FRAMEWORK} ${METAL_FRAMEWORK} ${FOUNDATION_FRAMEWORK})
endif()



# ----------------------------------------------------
# BUILD PLUGINS (.so files)
# ----------------------------------------------------
add_library(plugin_model_decryptor SHARED src/plugins/model_decryptor_plugin.cpp)
target_link_libraries(plugin_model_decryptor PRIVATE xinfer::xinfer)

add_library(plugin_yolo_nms SHARED src/plugins/yolo_nms_plugin.cpp)
target_link_libraries(plugin_yolo_nms PRIVATE xinfer::xinfer)

add_library(plugin_audio_melspectrogram SHARED src/plugins/audio_melspectrogram_plugin.cpp)
target_link_libraries(plugin_audio_melspectrogram PRIVATE xinfer::xinfer)

add_library(plugin_dynamic_quantizer SHARED src/plugins/dynamic_quantizer_plugin.cpp)
target_link_libraries(plugin_dynamic_quantizer PRIVATE xinfer::xinfer)

add_library(plugin_telemetry_profiler SHARED src/plugins/telemetry_profiler_plugin.cpp)
target_link_libraries(plugin_telemetry_profiler PRIVATE xinfer::xinfer)

add_library(plugin_softmax_topk SHARED src/plugins/softmax_topk_plugin.cpp)
target_link_libraries(plugin_softmax_topk PRIVATE xinfer::xinfer)

add_library(plugin_rga_resizer SHARED src/plugins/rga_resizer_plugin.cpp)
target_link_libraries(plugin_rga_resizer PRIVATE xinfer::xinfer)

add_library(plugin_dmabuf_allocator SHARED src/plugins/dmabuf_allocator_plugin.cpp)
target_link_libraries(plugin_dmabuf_allocator PRIVATE xinfer::xinfer)

add_library(plugin_tpm2_key_exchange SHARED src/plugins/tpm2_key_exchange_plugin.cpp)
target_link_libraries(plugin_tpm2_key_exchange PRIVATE xinfer::xinfer)

add_library(plugin_optical_flow SHARED src/plugins/optical_flow_plugin.cpp)
target_link_libraries(plugin_optical_flow PRIVATE xinfer::xinfer)

add_library(plugin_paged_attention SHARED src/plugins/paged_attention_plugin.cpp)
target_link_libraries(plugin_paged_attention PRIVATE xinfer::xinfer)

add_library(plugin_eta_packet SHARED src/plugins/eta_packet_plugin.cpp)
target_link_libraries(plugin_eta_packet PRIVATE xinfer::xinfer)

add_library(plugin_amp_fallback SHARED src/plugins/amp_fallback_plugin.cpp)
target_link_libraries(plugin_amp_fallback PRIVATE xinfer::xinfer)

add_library(plugin_lidar_voxelizer SHARED src/plugins/lidar_voxelizer_plugin.cpp)
target_link_libraries(plugin_lidar_voxelizer PRIVATE xinfer::xinfer)


# -------------------------------------------------------------------
# Subdirectories & Examples
# -------------------------------------------------------------------
if(XINFER_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(XINFER_BUILD_EXAMPLES)
    add_executable(xinfer_demo examples/01_tensorrt_yolo.cpp)
    target_link_libraries(xinfer_demo PRIVATE xinfer::xinfer)

    add_executable(demo_face_onnx examples/05_huggingface_face_detection.cpp)
    target_link_libraries(demo_face_onnx PRIVATE xinfer::xinfer)
endif()

# -------------------------------------------------------------------
# INSTALLATION RULES
# -------------------------------------------------------------------


# install(TARGETS xinfer DESTINATION /usr/local/lib)

install(TARGETS 
    xinfer 
    plugin_model_decryptor 
    plugin_yolo_nms 
    plugin_audio_melspectrogram 
    plugin_dynamic_quantizer 
    plugin_telemetry_profiler 
    plugin_softmax_topk 
    plugin_rga_resizer 
    plugin_dmabuf_allocator 
    plugin_tpm2_key_exchange 
    plugin_optical_flow 
    DESTINATION /usr/local/lib
)
install(DIRECTORY include/xinfer DESTINATION /usr/local/include)



# add_executable(demo_face_onnx examples/05_huggingface_face_detection.cpp)
# target_link_libraries(demo_face_onnx PUBLIC xinfer::xinfer)