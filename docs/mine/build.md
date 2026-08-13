# 1. Create build directory
mkdir build && cd build

# 2. Configure Shared Library & Tests
cmake .. -DXINFER_ENABLE_TENSORRT=ON \
         -DXINFER_BUILD_TESTS=ON \
         -DXINFER_BUILD_EXAMPLES=ON

# 3. Compile Shared Library (libxinfer.so)
make -j$(nproc)

# 4. Run All Unit Tests via CTest
ctest --output-on-failure

# 5. Run Benchmarking Binary manually
./tests/benchmark