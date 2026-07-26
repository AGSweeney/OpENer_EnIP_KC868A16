---
title: Platform
component: platform
level: platform
status: verified
topics:
  - esp-idf
  - esp32
  - ethernet-ip
---

# Platform

## Detected stack

| Layer | Inference | Evidence |
|-------|-----------|----------|
| Language | C11 | `.c`/`.h` sources, ESP-IDF |
| Build | CMake + ESP-IDF | root `CMakeLists.txt`, `idf.py` |
| Runtime | FreeRTOS on ESP32 | `freertos/task.h` in opener port |
| Targets | Kincony KC868-A16 (ESP32 + LAN8720) | `main/main.c`, README |
| Networking | EtherNet/IP (OpENer) + HTTP | opener, webui |
| Persistence | ESP-IDF NVS | `nvtcpip.c` |
| Concurrency | FreeRTOS tasks + HTTP handlers | OpENer task Core 0 |

## Target matrix

| Target | Built | Bench/CI verified | Notable features | Notes |
|--------|-------|-------------------|------------------|-------|
| KC868-A16 | Y | E4 | EIP adapter, WebUI | PoC — not production certified |

## Docs in this folder

- [build/build-instructions.md](build/build-instructions.md)
- [build/memory-configuration.md](build/memory-configuration.md)
- [device-identity-eds.md](device-identity-eds.md)
