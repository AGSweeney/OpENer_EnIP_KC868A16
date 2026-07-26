---
title: webui
component: webui
level: project
platforms:
  - ESP32-KC868-A16
topics:
  - http
  - ipconfig
  - dhcp
  - nvs
source_paths:
  - components/webui/include/webui.h
  - components/webui/src/webui.c
  - components/webui/src/webui_api.c
status: verified
retrieval:
  questions:
    - How do I change the device IP from a browser?
    - Which HTTP routes does the firmware expose?
    - How is g_tcpip protected against the OpENer task?
  related:
    - ../../../libraries/nvtcpip/README.md
    - ../ethernet-bringup/README.md
    - ../../../artifacts/data/ipconfig.http
---

# webui

Minimal ESP-IDF HTTP server for network configuration. Coupled to OpENer `g_tcpip` — **app-only**.

## Purpose

Serve `/` HTML and `/api/ipconfig` GET/POST for DHCP/static settings.

## Routes

| Method | Path | Handler |
|--------|------|---------|
| GET | `/` | Index HTML |
| GET | `/favicon.ico` | Icon |
| GET | `/api/ipconfig` | JSON from `g_tcpip` |
| POST | `/api/ipconfig` | Update + `NvTcpipStore` |

Examples: [A-P02-data](../../../artifacts/data/ipconfig.http).

## Ownership / concurrency

HTTP handlers take a mutex before touching `g_tcpip` ([A-P02-pat](../../../artifacts/patterns/webui_tcpip_mutex.c)). Started after `opener_init` from P03.

## Failure modes

| Symptom | Likely cause |
|---------|--------------|
| 500 timeout | Mutex held >1 s by OpENer path |
| Config not applied on wire | Device may need reboot after static IP change |

## Source evidence

| Claim | Evidence | Level |
|-------|----------|-------|
| max_uri_handlers = 5 | `webui.c` L138 | E1 |
| GET/POST /api/ipconfig | `webui_api.c` L336–361 | E1 |
| Mutex around g_tcpip | `webui_api.c` L36–38, L148–150 | E1 |
