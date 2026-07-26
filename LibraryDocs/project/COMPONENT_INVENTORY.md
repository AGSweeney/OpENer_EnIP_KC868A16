---
title: Component inventory
level: project
status: verified
inventory_version: 1
repo_root: .
last_updated: 2026-07-26
---

# Component Inventory

## Summary

| Metric | Count |
|--------|-------|
| Libraries | 4 |
| Project subsystems | 3 |
| Platform modules | 3 |
| Artifacts required | 12 |
| Verified (E1/E2) | 10 |
| Inferred/draft | 0 |

## Inventory table

| ID | Name | Level | Folder | Source paths | Reuse | Owner task | Socket/storage | Artifact IDs | Doc status | Evidence |
|----|------|-------|--------|--------------|-------|------------|----------------|--------------|------------|----------|
| L01 | i2c-manager | library | libraries/i2c-manager | components/i2c_manager/include/i2c_manager.h, components/i2c_manager/i2c_manager.c | high | OpENer / app init | I2C_NUM_0 bus | A-L01-if | verified | E1 |
| L02 | pcf8574 | library | libraries/pcf8574 | components/pcf8574/include/pcf8574.h, components/pcf8574/pcf8574.c | high | OpENer assembly callbacks | I2C devices 0x21–0x25 | A-L02-if, A-L02-pat | verified | E1 |
| L03 | opener-esp32 | library | libraries/opener-esp32 | components/opener/src/ports/ESP32/opener.c, components/opener/src/ports/ESP32/opener.h, components/opener/src/opener_api.h | medium | OpENer FreeRTOS task | TCP/UDP EtherNet/IP | A-L03-if, A-L03-pat | verified | E1 |
| L04 | nvtcpip | library | libraries/nvtcpip | components/opener/src/ports/nvdata/nvtcpip.h, components/opener/src/ports/nvdata/nvtcpip.c | medium | app_main / OpENer / HTTP | NVS namespace opener | A-L04-if, A-L04-data | verified | E1 |
| P01 | kc868-a16-application | project | project/subsystems/kc868-a16-application | components/opener/src/ports/ESP32/kc868_a16_application/kc868_a16_application.c, components/opener/src/ports/ESP32/devicedata.h | app-only | OpENer callbacks | Assemblies 100/150/151 | A-P01-pat, A-P01-data | verified | E1 |
| P02 | webui | project | project/subsystems/webui | components/webui/include/webui.h, components/webui/src/webui.c, components/webui/src/webui_api.c | app-only | HTTP server pool | HTTP /api/ipconfig, g_tcpip | A-P02-data, A-P02-pat | verified | E1 |
| P03 | ethernet-bringup | project | project/subsystems/ethernet-bringup | main/main.c | app-only | app_main | LAN8720 eth netif | A-P03-pat | verified | E1 |
| PL01 | Build/toolchain | platform | platform/build | CMakeLists.txt, sdkconfig.defaults, partitions.csv | n/a | — | flash / OTA | A-PL01-bld | verified | E1 |
| PL02 | Memory/partitions | platform | platform/build | partitions.csv, sdkconfig.defaults | n/a | — | NVS / OTA / SPIFFS | A-PL01-bld | verified | E1 |
| PL03 | Device identity & EDS | platform | platform | components/opener/src/ports/ESP32/devicedata.h, eds/KC868A16.eds | n/a | — | EDS / Identity object | A-PL03-data | verified | E1 |

### Artifact ID map

| ID | File |
|----|------|
| A-L01-if | interfaces/i2c_manager.h |
| A-L02-if | interfaces/pcf8574.h |
| A-L02-pat | patterns/pcf8574_requires_i2c_manager.c |
| A-L03-if | interfaces/opener_api_assembly.h |
| A-L03-pat | patterns/opener_init_task.c |
| A-L04-if | interfaces/nvtcpip.h |
| A-L04-data | data/tcpip_nv_blob.md |
| A-P01-pat | patterns/assembly_init.c |
| A-P01-data | data/assembly_layout.md |
| A-P02-data | data/ipconfig.http |
| A-P02-pat | patterns/webui_tcpip_mutex.c |
| A-P03-pat | patterns/got_ip_start_stack.c |
| A-PL01-bld | build/cmake_fd_setsize.cmake |
| A-PL03-data | data/eds_connection_path.md |

## Excluded (grouped under parent)

| Symbol/file | Parent ID | Reason |
|-------------|-----------|--------|
| components/lwip/** | PL01 | Vendored ESP-IDF lwIP; not project-owned API |
| components/esp_netif/** | PL01 | Vendored ESP-IDF netif glue |
| opener CIP core (cip*.c) | L03 | Documented via opener_api.h application surface |
| webui_html.c | P02 | Static HTML string; no separate API |
| opener_error.c | L03 | Thin errno helper |

## Coupling register

| From ID | To ID | Coupling type | Notes |
|---------|-------|---------------|-------|
| P01 | L01 | calls API | ApplicationInitialization → i2c_manager_init |
| P01 | L02 | calls API | UpdateInputs/UpdateOutputs via pcf8574_read/write |
| P01 | L03 | calls API | CreateAssemblyObject, Configure*ConnectionPoint |
| P03 | L03 | calls API | got_ip → opener_init(lwip_netif) |
| P03 | P02 | calls API | got_ip → webui_init after opener_init |
| P02 | L04 | calls API | POST /api/ipconfig → NvTcpipStore |
| P02 | L03 | global state | Reads/writes g_tcpip under mutex |
| L03 | L04 | calls API | opener_init loads NvTcpipLoad; SetCallback stores |
| L02 | L01 | calls API | pcf8574_init requires i2c_manager_is_initialized |
| P01 | PL03 | config blob | Identity macros / assembly IDs must match EDS |

## Retrieval keywords

| ID | keywords |
|----|----------|
| L01 | i2c, bus, GPIO4, GPIO5, i2c_manager_init |
| L02 | pcf8574, expander, relay, opto input, active-low |
| L03 | OpENer, EtherNet/IP, CipStackInit, opener_init, FreeRTOS |
| L04 | NVS, tcpip_cfg, NvTcpipLoad, NvTcpipStore, hostname |
| P01 | assembly 100, assembly 150, assembly 151, KC868-A16, ADC |
| P02 | webui, /api/ipconfig, HTTP, DHCP, static IP |
| P03 | LAN8720, eth, got_ip, RMII, app_main |
| PL01 | ESP-IDF, idf.py, FD_SETSIZE, MINIMAL_BUILD |
| PL02 | partitions, OTA, NVS, 4MB flash |
| PL03 | EDS, VendorId 55512, connection path, config assembly |
