---
title: Memory configuration
component: memory-partitions
level: platform
platforms:
  - ESP32-KC868-A16
topics:
  - partitions
  - ota
  - nvs
  - flash
source_paths:
  - partitions.csv
  - sdkconfig.defaults
  - components/opener/src/ports/ESP32/opener.c
status: verified
---

# Memory configuration

## Flash partitions (4MB)

| Name | Type | Offset | Size |
|------|------|--------|------|
| nvs | data | 0x9000 | 20 KB |
| otadata | data | 0xe000 | 8 KB |
| ota_0 | app | 0x10000 | 1.5 MB |
| ota_1 | app | 0x190000 | 1.5 MB |
| spiffs | data | 0x310000 | 960 KB |

## Runtime

| Resource | Value | Evidence |
|----------|-------|----------|
| OpENer task stack | 8192 words/bytes per FreeRTOS config | `opener.c` `OPENER_STACK_SIZE` |
| OpENer priority | 5 | `OPENER_THREAD_PRIO` |
| Core affinity | Core 0 | `xTaskCreatePinnedToCore(..., 0)` |

## Source evidence

| Claim | Evidence | Level |
|-------|----------|-------|
| Partition table sizes | `partitions.csv` | E1 |
| 4MB flash default | `sdkconfig.defaults` L10–11 | E1 |
| OpENer stack 8192 | `opener.c` L24 | E1 |
