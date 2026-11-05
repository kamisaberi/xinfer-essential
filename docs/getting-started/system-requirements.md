# System Requirements & Prerequisites

Review the toolchain, operating system, and silicon driver requirements before compiling or running `xinfer-essential`.

---

## 1. Host Operating System & Architecture

`xinfer-essential` requires a 64-bit POSIX-compliant operating system with modern kernel DMA-BUF and asynchronous I/O capabilities.

| Operating System | Minimum Version | Recommended Target | Supported Architectures |
| :--- | :--- | :--- | :--- |
| **Ubuntu Linux** | 22.04 LTS | 24.04 LTS / 26.04 Devel | `x86_64`, `aarch64` |
| **Debian** | 12 (Bookworm) | 12 (Bookworm) | `x86_64`, `aarch64` |
| **Red Hat Enterprise Linux** | 9.0 | 9.4 | `x86_64`, `aarch64` |
| **macOS** | 13.0 (Ventura) | 14.0+ (Sonoma) | `arm64` (Apple Silicon) |

---

## 2. Compiler & Toolchain Standards

* **GCC:** Version **12.1.0** or newer.
* **Clang/LLVM:** Version **16.0.0** or newer.
* **CMake:** Version **3.24.0** or newer.
* **Ninja Build:** Version **1.10.0+**.
* **glibc:** Version **2.35+**.
