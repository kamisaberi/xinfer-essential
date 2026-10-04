---

### File: `xinfer-essential/docs/troubleshooting/out-of-memory-diagnostics.md`

```markdown
# Out-of-Memory (OOM) & DMA Allocation Diagnostics

This guide addresses memory locking, DMA allocation, and buffer pool failures in constrained edge environments.

---

## 1. Locked Memory Limit Exceeded (`ERR_MEMORY_PIN_FAILED`)

### Symptom
```text
[xInfer FATAL] mlock() failed with errno = 12 (Cannot allocate memory)
[xInfer Exception] ERR_MEMORY_PIN_FAILED: Memory locking limit exceeded
```

### Cause
The Linux security limit `RLIMIT_MEMLOCK` restricts the number of RAM pages an unprivileged process can lock into physical memory.

### Remediation
1. Inspect current process limits:
   ```bash
   ulimit -l
   ```
2. Update `/etc/security/limits.conf` to grant unlimited locked memory:
   ```text
   *    soft    memlock    unlimited
   *    hard    memlock    unlimited
   ```
3. For systemd services, add the following to the unit file:
   ```ini
   [Service]
   LimitMEMLOCK=infinity
   ```

---

## 2. Linux Kernel CMA / DMA-Heap Exhaustion

### Symptom
```text
[xInfer Exception] ERR_OUT_OF_MEMORY: Kernel DMA heap allocation failed (/dev/dma_heap/system)
dmesg output: cma: cma_alloc: alloc failed, req-size: 1024 pages, ret: -12
```

### Cause
Embedded vision systems and DMA-BUF zero-copy pipelines require contiguous physical memory reserved via the kernel Contiguous Memory Allocator (CMA). If the CMA pool is exhausted by camera drivers or display framebuffers, allocations fail.

### Remediation
Increase the kernel CMA reservation by modifying `/boot/cmdline.txt` (on ARM) or `/etc/default/grub` (on x86):

```text
cma=512M
```

Reboot and verify the allocated pool size:
```bash
cat /proc/meminfo | grep -i cma
```

---

## 3. Diagnosing IOMMU Faults

### Symptom
High-bandwidth DMA-BUF execution freezes the machine or emits kernel page faults:
```text
dmesg output: DMAR: DRHD: handling fault status reg 2
dmesg output: DMAR: [DMA Read] Request device [01:00.0] fault addr 0x...
```

### Remediation
Verify whether the IOMMU is running in pass-through mode:
```text
GRUB_CMDLINE_LINUX_DEFAULT="intel_iommu=on iommu=pt"
```
Setting `iommu=pt` enables hardware translation pass-through for high-throughput PCIe network and accelerator devices while preventing false page protection faults.
```

---

### File: `xinfer-essential/docs/troubleshooting/faq.md`

```markdown
# Technical Frequently Asked Questions (FAQ)

---

### Q1: Can I load PyTorch `.pt` or TensorFlow SavedModel files directly into `xinfer`?
**No.** `xinfer-essential` is designed for low-latency operational environments and avoids embedding Python interpreters or full training frameworks. Models must be exported to standardized execution formats—such as **ONNX (Opset 11–17)**, OpenVINO IR, TensorRT `.engine`, or vendor binaries (`.rknn`, `.hef`)—before deployment.

---

### Q2: How does `xinfer` achieve zero heap allocations in the forward pass?
All tensor backing buffers, scratchpads, dynamic memory queues, and device descriptors are allocated and pinned during `InferenceEngine::initialize()`. During `forward()`, inputs are processed directly in-place through pre-mapped memory buffers, invoking zero calls to `malloc()`, `free()`, `new`, or `delete`.

---

### Q3: What happens if an edge hardware accelerator crashes or overheats?
`xinfer-essential` isolates driver errors at the `IInferencePlugin` boundary. If an accelerator encounters a hardware fault (e.g., PCIe timeout or driver crash), the engine catches the low-level failure, releases driver resources safely, and throws a typed `InferenceException`. If configured, it automatically redirects the next inference pass to the optimized SIMD CPU reference backend.

---

### Q4: Does `xinfer` transmit any metrics or telemetry over the internet?
**No.** `xinfer-essential` enforces a $\$0.00$ cloud egress model. It creates zero background threads, telemetry sockets, or external network connections. External HTTPS communication occurs only if you explicitly invoke `xinfer::ModelHub` with remote synchronization enabled. In air-gapped deployments, setting `enable_offline_mode = true` ensures zero socket operations occur.

---

### Q5: How can I achieve $< 1\,\mu\text{s}$ latency on CPU execution?
To achieve sub-microsecond latency on CPUs (such as Intel Xeon or Core Ultra):
1. Use models with small parameter footprints (e.g., 32-dim tabular autoencoders).
2. Compile with AVX-512 enabled (`-march=native`).
3. Isolate the execution CPU core using the Linux kernel boot parameter `isolcpus=<core_id>`.
4. Lock CPU core frequency to maximum using `cpupower frequency-set -g performance`.
5. Pre-warm CPU caches with at least $1{,}000$ discard passes.
```

---

### File: `xinfer-essential/docs/troubleshooting/support.md`

```markdown
# Enterprise Support & Issue Reporting

---

## 1. Reporting Bugs & Technical Inquiries

When reporting a runtime issue or suspected regression on GitHub, include output from the automated diagnostics tool along with your environment details:

```bash
# Generate comprehensive hardware diagnostics
xinfer-diag --verbose > xinfer_diag.log

# Capture kernel environment
uname -a >> xinfer_diag.log
dmesg | tail -n 100 >> xinfer_diag.log
```

Open an issue on GitHub:
👉 **[https://github.com/kamisaberi/xinfer-essential/issues](https://github.com/kamisaberi/xinfer-essential/issues)**

---

## 2. Enterprise Support SLAs

Aryorithm Technologies B.V. provides commercial support for mission-critical industrial and defense deployments:

| Support Tier | Target SLA | Coverage | Scope |
| :--- | :--- | :--- | :--- |
| **Community Support** | Best Effort | Public GitHub Issues | Bug reports, public docs. |
| **Enterprise Standard**| 8 Business Hours | Private Help Desk | Integration reviews, performance audits. |
| **Mission-Critical Defense**| **1 Hour (24/7/365)** | Dedicated Systems Architect | On-site triage, kernel driver engineering, custom silicon BSP porting. |

For enterprise support contracts and air-gapped security certification inquiries:
* **Email:** `support@aryorithm.com`
* **Website:** `https://aryorithm.com`

---

## 3. Security Vulnerability Disclosure Policy

If you discover a security vulnerability or potential memory safety issue in `xinfer-essential`, do not open a public issue.

Send an encrypted report to **`security@aryorithm.com`**. We acknowledge disclosures within **48 hours** and provide CVE assignment, risk remediation, and backported security patches according to coordinated disclosure guidelines.
```

---

## Master Status: Project 1 Documentation Complete

The documentation suite for **Project 1 of 8: `xinfer-essential` (`libxinfer.so`)** is fully generated:

```text
xinfer-essential/docs/
├── mkdocs.yml                                 # [Delivered Part 1]
├── index.md                                   # [Delivered Part 1]
├── getting-started/ (6 files)                 # [Delivered Part 1]
├── architecture/ (5 files)                    # [Delivered Part 2]
├── silicon-backends/ (16 files)               # [Delivered Parts 3, 4, 5]
├── memory-management/ (5 files)               # [Delivered Part 6]
├── plugin-development/ (4 files)              # [Delivered Part 7]
│   └── standard-plugins/ (9 files)            # [Delivered Part 8]
├── model-hub/ (5 files)                       # [Delivered Part 9]
├── api-reference/ (8 files)                   # [Delivered Part 10]
├── tutorials/ (5 files)                       # [Delivered Part 11]
├── benchmarking/ (5 files)                    # [Delivered Part 12]
└── troubleshooting/ (6 files)                 # [Delivered Part 13]
```

Total: **70 documentation and configuration files**, providing full technical coverage of the Tier 1 inference engine.

---

### Ready for Next Project

When you are ready, confirm to transition to **Project 2 of 8**:  
👉 **`blackbox-essential` (`libblackbox.so`)** — *Tier 2 Active Mitigation Core (Linux eBPF/XDP Kernel Filter, SPMC Event Ring Buffer, and TPM 2.0 Hardware Root of Trust).*