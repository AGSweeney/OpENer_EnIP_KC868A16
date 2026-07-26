---
title: nvtcpip
component: nvtcpip
level: library
reuse: medium
platforms:
  - ESP32-KC868-A16
topics:
  - nvs
  - tcpip
  - persistence
  - dhcp
source_paths:
  - components/opener/src/ports/nvdata/nvtcpip.h
  - components/opener/src/ports/nvdata/nvtcpip.c
status: verified
retrieval:
  questions:
    - Where is TCP/IP configuration persisted?
    - What is the NVS namespace and key for OpENer IP settings?
    - How does NvTcpipLoad treat missing configuration?
  related:
    - ../opener-esp32/README.md
    - ../../project/subsystems/webui/README.md
    - ../../artifacts/data/tcpip_nv_blob.md
---

# nvtcpip

ESP-IDF NVS persistence for OpENer `CipTcpIpObject` (`g_tcpip`). **Reuse: medium**.

## Purpose

Load/store IP method, address fields, hostname/domain, and ACD select across reboot.

## Reuse classification

Medium — tied to OpENer `CipTcpIpObject` layout and NVS namespace `opener`.

## Public API

[A-L04-if](../../artifacts/interfaces/nvtcpip.h): `NvTcpipLoad`, `NvTcpipStore`.

## Dependencies

`nvs_flash`, `ciptcpipinterface.h`, lwIP address helpers.

## Ownership / concurrency

Callers must serialize access to `g_tcpip` (WebUI uses a mutex). Store is invoked from CIP Set callback and HTTP POST.

## Runtime lifecycle

1. `nvs_flash_init` in `app_main` (P03).
2. Early `NvTcpipLoad` in `app_main` to choose DHCP vs static before eth start.
3. `opener_init` loads again into stack object.
4. Changes via CIP or WebUI call `NvTcpipStore`.

## Configuration

Blob schema: [A-L04-data](../../artifacts/data/tcpip_nv_blob.md). Version 2 includes `select_acd`.

## Initialization

Missing namespace/key → `kEipStatusError` (caller uses defaults/DHCP).

## Error handling

Logs `esp_err_t` from NVS; returns `kEipStatusError` on failure.

## Thread safety

NVS APIs are used without an internal lock; rely on caller mutex for object coherency.

## Memory / resources

Stack-allocated packed blob (`TcpipNvBlob`); hostname/domain length-capped.

## Limits

Hostname max 64; domain max 48 bytes.

## Failure modes

| Symptom | Likely cause |
|---------|--------------|
| Always DHCP after flash erase | No blob — expected |
| Truncated hostname | Length > 64 |

## Data formats

Packed little-endian struct versioned at `TCPIP_NV_VERSION` 2. Canonical: [tcpip_nv_blob.md](../../artifacts/data/tcpip_nv_blob.md).

## Integration points

P03 early load; L03 init load; P02 store on POST; CIP Set callback `NvTcpipSetCallback`.

## Logging / diagnostics

Tag `NvTcpip`.

## Portability

ESP-IDF NVS only.

## Security considerations

No encryption of NVS blob; physical access can read IP config.

## Related components

- [opener-esp32](../opener-esp32/README.md)
- [webui](../../project/subsystems/webui/README.md)

## Source evidence

| Claim | Evidence | Level |
|-------|----------|-------|
| Namespace `opener`, key `tcpip_cfg` | `nvtcpip.c` L26–27 | E1 |
| Version 2 blob fields | `nvtcpip.c` L35–48 | E1 |
| Missing blob returns error | `nvtcpip.c` L84–86 | E1 |
