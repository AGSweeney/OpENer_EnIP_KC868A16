---
title: Build instructions
component: build-toolchain
level: platform
platforms:
  - ESP32-KC868-A16
topics:
  - esp-idf
  - cmake
  - idf.py
  - flash
source_paths:
  - CMakeLists.txt
  - sdkconfig.defaults
  - README.md
status: verified
---

# Build instructions

## Prerequisites

ESP-IDF 5.5.1+, Python 3.8+, CMake 3.16+.

## Build / flash

```bash
. $HOME/esp/esp-idf/export.sh   # or Windows ESP-IDF export
idf.py build
idf.py flash
idf.py monitor
```

## Notable CMake flags

[A-PL01-bld](../../artifacts/build/cmake_fd_setsize.cmake):

- `MINIMAL_BUILD ON`
- `FD_SETSIZE=30` for lwIP socket set size
- Post-build copy of `.bin` into `FirmwareImages/` via `scripts/copy_firmware.py`

## Defaults

`sdkconfig.defaults` enables custom `partitions.csv` and 4MB flash.

## Source evidence

| Claim | Evidence | Level |
|-------|----------|-------|
| MINIMAL_BUILD + FD_SETSIZE | `CMakeLists.txt` L11–17 | E1 |
| Custom partition table | `sdkconfig.defaults` L1–3 | E1 |
| Project name KC868_A16_EnIP | `CMakeLists.txt` L19 | E1 |
