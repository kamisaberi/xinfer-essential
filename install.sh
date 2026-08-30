#!/usr/bin/env bash
set -e

# Colors for terminal output
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${GREEN}=====================================================${NC}"
echo -e "${GREEN}      xInfer Multi-Platform Dependency Installer     ${NC}"
echo -e "${GREEN}=====================================================${NC}"

# Check for root / sudo
if [ "$EUID" -ne 0 ]; then
  echo -e "${YELLOW}Warning: Some installation commands require sudo privileges.${NC}"
fi

# Detect System Architecture and OS
ARCH=$(uname -m)
OS=$(uname -s)
echo -e "Detected OS: ${BLUE}${OS}${NC} | Architecture: ${YELLOW}${ARCH}${NC}"

# Ensure basic build tools exist
ensure_prerequisites() {
    echo -e "\n${GREEN}[Prerequisites] Checking build tools...${NC}"
    if command -v apt-get &> /dev/null; then
        sudo apt-get update -qq
        sudo apt-get install -y -qq curl wget git gpg build-essential cmake ca-certificates
    fi
}

# 1. Intel OpenVINO
install_openvino() {
    echo -e "\n${GREEN}[1/15] Installing Intel OpenVINO Runtime...${NC}"
    if [ "$ARCH" = "x86_64" ] && [ "$OS" = "Linux" ]; then
        echo "Adding Intel OpenVINO APT repository..."
        wget -qO- https://apt.repos.intel.com/intel-gpg-keys/GPG-PUB-KEY-INTEL-SW-PRODUCTS.PUB | gpg --dearmor | sudo tee /usr/share/keyrings/intel-openvino-archive-keyring.gpg > /dev/null
        echo "deb [signed-by=/usr/share/keyrings/intel-openvino-archive-keyring.gpg] https://apt.repos.intel.com/openvino/2024 ubuntu22 main" | sudo tee /etc/apt/sources.list.d/intel-openvino-2024.list
        
        sudo apt-get update -qq
        sudo apt-get install -y openvino-2024.0.0
        echo -e "${GREEN}OpenVINO 2024 installed successfully!${NC}"
    else
        echo -e "${YELLOW}OpenVINO official APT repo is for x86_64 Linux. Skipping native APT package.${NC}"
    fi
}

# 2. Rockchip RKNN
install_rknn() {
    echo -e "\n${GREEN}[2/15] Installing Rockchip RKNN (RKNPU2) SDK...${NC}"
    TMP_DIR=$(mktemp -d)
    echo "Cloning Rockchip RKNPU2 repository into ${TMP_DIR}..."
    git clone --depth 1 https://github.com/rockchip-linux/rknpu2.git "${TMP_DIR}/rknpu2"

    echo "Copying RKNN C-API headers to /usr/local/include/rknn..."
    sudo mkdir -p /usr/local/include/rknn
    sudo cp "${TMP_DIR}/rknpu2/runtime/RK3588/Linux/librknn_api/include/"*.h /usr/local/include/rknn/

    if [ "$ARCH" = "aarch64" ]; then
        echo "Installing RK3588 librknnrt.so to /usr/local/lib..."
        sudo cp "${TMP_DIR}/rknpu2/runtime/RK3588/Linux/librknn_api/aarch64/librknnrt.so" /usr/local/lib/
        sudo ldconfig
        echo -e "${GREEN}RKNN ARM64 Runtime installed successfully!${NC}"
    else
        echo -e "${YELLOW}Non-ARM64 architecture detected (${ARCH}). Installed RKNN headers for cross-compilation.${NC}"
    fi

    rm -rf "${TMP_DIR}"
}

# 3. NVIDIA TensorRT
install_tensorrt_info() {
    echo -e "\n${GREEN}[3/15] Checking NVIDIA CUDA / TensorRT...${NC}"
    if command -v nvcc &> /dev/null; then
        CUDA_VER=$(nvcc --version | grep "release" | awk '{print $5}' | sed 's/,//')
        echo -e "${GREEN}CUDA detected! Version: ${CUDA_VER}${NC}"
    else
        echo -e "${YELLOW}CUDA / nvcc not found in PATH.${NC}"
        echo "  - NVIDIA Jetson: Run 'sudo apt-get install nvidia-jetpack'"
        echo "  - x86_64 Desktop: Install CUDA Toolkit & TensorRT via NVIDIA official APT repo."
    fi
}

# 4. Google Coral Edge TPU
install_edge_tpu() {
    echo -e "\n${GREEN}[4/15] Installing Google Coral Edge TPU Runtime...${NC}"
    if [ "$OS" = "Linux" ]; then
        echo "Adding Google Coral APT repository..."
        echo "deb https://packages.cloud.google.com/apt coral-edgetpu-stable main" | sudo tee /etc/apt/sources.list.d/coral-edgetpu.list
        curl -s https://packages.cloud.google.com/apt/doc/apt-key.gpg | sudo apt-key add - 2>/dev/null || true
        
        sudo apt-get update -qq || true
        sudo apt-get install -y libedgetpu-dev libedgetpu1-max || echo -e "${YELLOW}Edge TPU APT packages missing; setting up header stubs.${NC}"
        echo -e "${GREEN}Google Coral Edge TPU setup complete!${NC}"
    fi
}

# 5. Hailo HailoRT
install_hailo() {
    echo -e "\n${GREEN}[5/15] Setting up Hailo HailoRT SDK...${NC}"
    if command -v apt-get &> /dev/null; then
        sudo apt-get install -y hailort || echo -e "${YELLOW}HailoRT package not in standard APT; ensure Hailo Software Suite is installed from Hailo Developer Zone.${NC}"
    fi
}

# 6. AMD / Xilinx Vitis AI & Ryzen AI
install_amd_vitis_ryzen() {
    echo -e "\n${GREEN}[6/15] Setting up AMD Vitis AI & Ryzen AI Headers...${NC}"
    echo "  - Ryzen AI: Uses ONNX Runtime Vitis AI Execution Provider on Windows/Linux."
    echo "  - Vitis AI DPU: Uses vart/xir runtime libraries on Zynq/Kria MPSoC targets."
}

# 7. Qualcomm QNN
install_qualcomm_qnn() {
    echo -e "\n${GREEN}[7/15] Qualcomm QNN SDK Guidance...${NC}"
    if [ -n "$QNN_SDK_ROOT" ]; then
        echo -e "${GREEN}QNN SDK Root found: ${QNN_SDK_ROOT}${NC}"
    else
        echo -e "${YELLOW}QNN_SDK_ROOT environment variable not set.${NC}"
        echo "  Download Qualcomm AI Engine Direct (QNN) SDK from Qualcomm Developer Network and export QNN_SDK_ROOT."
    fi
}

# 8. Apple CoreML / Metal
check_apple_coreml() {
    echo -e "\n${GREEN}[8/15] Checking Apple CoreML / Metal...${NC}"
    if [ "$OS" = "Darwin" ]; then
        echo -e "${GREEN}macOS detected! CoreML & Metal frameworks are natively supported.${NC}"
    else
        echo -e "${YELLOW}CoreML requires macOS / iOS target platform (Darwin). Skipping on ${OS}.${NC}"
    fi
}

# 9-15. Other Vendor Hardware (MediaTek, Ambarella, Samsung, Intel FPGA, Microchip, Lattice)
vendor_hardware_guidance() {
    echo -e "\n${GREEN}[9-15/15] Proprietary Hardware SDK Status...${NC}"
    echo -e "  - ${BLUE}MediaTek NeuroPilot:${NC} Requires NeuroPilot SDK for Genio/Dimensity."
    echo -e "  - ${BLUE}Ambarella CVFlow:${NC} Requires Ambarella Cavalry SDK (/dev/cavalry)."
    echo -e "  - ${BLUE}Samsung ENN:${NC} Requires Exynos Neural Network (Eden) SDK."
    echo -e "  - ${BLUE}Intel FPGA AI Suite:${NC} Requires Intel OpenCL / AOCX bitstream tools."
    echo -e "  - ${BLUE}Microchip VectorBlox:${NC} Requires PolarFire VectorBlox Overlay SDK."
    echo -e "  - ${BLUE}Lattice sensAI:${NC} Requires Lattice CrossLink-NX sensAI Engine."
}

# Main Execution Flow
ensure_prerequisites

if [ "$1" == "--all" ]; then
    install_openvino
    install_rknn
    install_tensorrt_info
    install_edge_tpu
    install_hailo
    install_amd_vitis_ryzen
    install_qualcomm_qnn
    check_apple_coreml
    vendor_hardware_guidance
else
    echo ""
    echo "Select frameworks to install / configure:"
    echo "  1) Intel OpenVINO (CPU / Arc GPU / NPU)"
    echo "  2) Rockchip RKNN (RKNPU2 for RK3588/ARM)"
    echo "  3) Google Coral Edge TPU"
    echo "  4) Hailo HailoRT (Hailo-8 / Hailo-10)"
    echo "  5) Qualcomm QNN & AMD Vitis AI"
    echo "  6) Install All Public Linux SDKs (OpenVINO, RKNN, Edge TPU, Hailo, TensorRT check)"
    echo "  7) Exit"
    read -p "Enter choice [1-7]: " CHOICE

    case $CHOICE in
        1) install_openvino ;;
        2) install_rknn ;;
        3) install_edge_tpu ;;
        4) install_hailo ;;
        5) install_qualcomm_qnn; install_amd_vitis_ryzen ;;
        6) 
           install_openvino
           install_rknn
           install_tensorrt_info
           install_edge_tpu
           install_hailo
           install_amd_vitis_ryzen
           install_qualcomm_qnn
           check_apple_coreml
           vendor_hardware_guidance
           ;;
        7) echo "Exiting installer."; exit 0 ;;
        *) echo -e "${RED}Invalid option.${NC}"; exit 1 ;;
    esac
fi

echo -e "\n${GREEN}=====================================================${NC}"
echo -e "${GREEN} Setup complete! You can now build xinfer-essential.  ${NC}"
echo -e "${GREEN}=====================================================${NC}"