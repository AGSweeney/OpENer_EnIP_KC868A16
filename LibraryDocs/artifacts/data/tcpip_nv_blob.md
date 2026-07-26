---
title: TCP/IP NVS blob layout
component: nvtcpip
level: library
reuse: medium
status: verified
topics:
  - nvs
  - tcpip
  - persistence
---

# TCP/IP NVS blob layout

Canonical packed layout from `components/opener/src/ports/nvdata/nvtcpip.c`.

| Field | Type | Notes |
|-------|------|-------|
| version | uint32 | Current `TCPIP_NV_VERSION` = 2 |
| config_control | uint32 | DHCP vs static method bits |
| ip_address | uint32 | Network byte order |
| network_mask | uint32 | |
| gateway | uint32 | |
| name_server | uint32 | |
| name_server2 | uint32 | |
| domain_length | uint16 | Max 48 |
| hostname_length | uint16 | Max 64 |
| domain | uint8[48] | |
| hostname | uint8[64] | |
| select_acd | uint8 | Present in v2 only |

| Constant | Value |
|----------|-------|
| Namespace | `opener` |
| Key | `tcpip_cfg` |

V1 blobs omit `select_acd`; loader migrates on read.

## Source evidence

| Claim | Evidence | Level |
|-------|----------|-------|
| Namespace/key and v2 struct | `components/opener/src/ports/nvdata/nvtcpip.c` L26–48 | E1 |
