#!/usr/bin/env bash
set -e

# Colors for terminal output
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color

echo -e "${GREEN}=====================================================${NC}"
echo -e "${GREEN}      xInfer Essential Dependency Installer          ${NC}"
echo -e "${GREEN}=====================================================${NC}"

# Check for root / sudo
if [ "$EUID" -ne 0 ]; then
  echo -e "${YELLOW}Warning: Some installation commands require sudo privileges.${NC}"
fi

# Detect System Architecture
ARCH=$(uname -m)
echo -e "Detected System Architecture: ${YELLOW}${ARCH}${NC}"

install_openvino() {
    echo -e "\n${GREEN}[1/3] Installing Intel OpenVINO Runtime...${NC}"
    if [ "$ARCH" = "x86_64" ]; then
        echo "Adding Intel OpenVINO APT repository..."
        wget -qO- https://apt.repos.intel.com/intel-gpg-keys/GPG-PUB-KEY-INTEL-SW-PRODUCTS.PUB | gpg --dearmor | sudo tee /usr/share/keyrings/intel-openvino-archive-keyring.gpg > /dev/null
        echo "deb [signed-by=/usr/share/keyrings/intel-openvino-archive-keyring.gpg] https://apt.repos.intel.com/openvino/2024 ubuntu22 main" | sudo tee /etc/apt/sources.list.d/intel-openvino-2024.list
        
        sudo apt-get update
        sudo apt-get install -y openvino-2024.0.0
        echo -e "${GREEN}OpenVINO 2024 installed successfully!${NC}"
    else
        echo -e "${RED}OpenVINO official APT repository is primarily supported on x86_64.${NC}"
    fi
}

install_rknn() {
    echo -e "\n${GREEN}[2/3] Installing Rockchip RKNN (RKNPU2) SDK...${NC}"
    TMP_DIR=$(mktemp -d)
    echo "Cloning Rockchip RKNPU2 repository into ${TMP_DIR}..."
    git clone --depth 1 https://github.com/rockchip-linux/rknpu2.git "${TMP_DIR}/rknpu2"

    echo "Copying RKNN C-API headers to /usr/local/include..."
    sudo mkdir -p /usr/local/include/rknn
    sudo cp "${TMP_DIR}/rknpu2/runtime/RK3588/Linux/librknn_api/include/"*.h /usr/local/include/rknn/

    if [ "$ARCH" = "aarch64" ]; then
        echo "Installing RK3588 librknnrt.so to /usr/local/lib..."
        sudo cp "${TMP_DIR}/rknpu2/runtime/RK3588/Linux/librknn_api/aarch64/librknnrt.so" /usr/local/lib/
        sudo ldconfig
        echo -e "${GREEN}RKNN ARM64 Runtime installed successfully!${NC}"
    else
        echo -e "${YELLOW}Non-ARM64 architecture detected (${ARCH}). Installed RKNN C-API headers for cross-compilation.${NC}"
    fi

    rm -rf "${TMP_DIR}"
}

install_tensorrt_info() {
    echo -e "\n${GREEN}[3/3] NVIDIA TensorRT Check...${NC}"
    if command -v nvcc &> /dev/null; then
        CUDA_VER=$(nvcc --version | grep "release" | awk '{print $5}' | sed 's/,//')
        echo -e "${GREEN}CUDA is detected! Version: ${CUDA_VER}${NC}"
    else
        echo -e "${YELLOW}CUDA / nvcc not found in PATH.${NC}"
        echo "To install TensorRT & CUDA on Ubuntu:"
        echo "  1. On NVIDIA Jetson: Run 'sudo apt-get install nvidia-jetpack'"
        echo "  2. On x86_64 Desktop: Install CUDA Toolkit & TensorRT via NVIDIA official APT repo."
    fi
}

# Menu selection
if [ "$1" == "--all" ]; then
    install_openvino
    install_rknn
    install_tensorrt_info
else
    echo ""
    echo "Select frameworks to install:"
    echo "  1) Intel OpenVINO (CPU / Arc / NPU)"
    echo "  2) Rockchip RKNN (RKNPU2 for RK3588/ARM)"
    echo "  3) All of the above"
    echo "  4) Exit"
    read -p "Enter choice [1-4]: " CHOICE

    case $CHOICE in
        1) install_openvino ;;
        2) install_rknn ;;
        3) install_openvino; install_rknn; install_tensorrt_info ;;
        4) echo "Exiting installer."; exit 0 ;;
        *) echo -e "${RED}Invalid option.${NC}"; exit 1 ;;
    esac
fi

echo -e "\n${GREEN}=====================================================${NC}"
echo -e "${GREEN} Setup complete! You can now build xinfer-essential.  ${NC}"
echo -e "${GREEN}=====================================================${NC}"