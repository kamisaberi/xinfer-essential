
### How Developers Use This:

#### Option A: One-Command Installation via `install.sh`
```bash
./install.sh --all
```

#### Option B: Build via CMake (Automatic Header Fetching)
```bash
mkdir build && cd build
cmake .. -DXINFER_ENABLE_RKNN=ON
make -j$(nproc)
```
If RKNN headers are missing on their machine, CMake will clone them automatically during compilation!