---
title: opener-esp32
component: opener-esp32
level: library
reuse: medium
platforms:
  - ESP32-KC868-A16
topics:
  - opener
  - ethernet-ip
  - cip
  - freertos
  - assembly
source_paths:
  - components/opener/src/ports/ESP32/opener.c
  - components/opener/src/ports/ESP32/opener.h
  - components/opener/src/opener_api.h
status: verified
retrieval:
  questions:
    - How do I start the OpENer stack on ESP32?
    - Which FreeRTOS core and stack size does OpENer use?
    - Which application callbacks must I implement for assemblies?
  related:
    - ../nvtcpip/README.md
    - ../../project/subsystems/kc868-a16-application/README.md
    - ../../project/architecture/system-overview.md
---

# opener-esp32

ESP32 FreeRTOS port entry for the OpENer EtherNet/IP adapter stack, plus the application-facing CIP API. **Reuse: medium** (ESP32/lwIP-specific init; CIP API is portable).

## Purpose

Initialize CIP stack, bind TCP/IP NV callbacks, and run `NetworkHandlerProcessCyclic` on a dedicated task.

## Reuse classification

Medium — `opener_init(struct netif *)` is ESP32/lwIP specific; `CreateAssemblyObject` / connection-point APIs are standard OpENer.

## Public API

- Port: `opener_init(struct netif *netif)` — [opener.h](../../../components/opener/src/ports/ESP32/opener.h)
- Application surface: [A-L03-if](../../artifacts/interfaces/opener_api_assembly.h)
- Init pattern: [A-L03-pat](../../artifacts/patterns/opener_init_task.c)

## Dependencies

lwIP `netif`, FreeRTOS, OpENer CIP core, [nvtcpip](../nvtcpip/README.md), application callbacks in P01.

## Ownership / concurrency

Task `"OpENer"` pinned to **Core 0**, priority **5**, stack **8192**. Owns CIP connections and socket select loop. WebUI must not race `g_tcpip` without mutex (see P02).

## Runtime lifecycle

1. Caller waits for Ethernet link + IP (`got_ip`).
2. `opener_init` → `CipStackInit` → `NetworkHandlerInitialize` → create task.
3. Task loops `NetworkHandlerProcessCyclic` until link down / error.
4. `NetworkHandlerFinish` + `ShutdownCipStack` + `vTaskDelete`.

## Configuration

Identity macros in `devicedata.h` (see PL03). Connection points configured by application `ApplicationInitialization`.

## Initialization

Mutex-guarded; skips if already initialized. Requires `IfaceLinkIsUp(netif)`.

## Error handling

`EipStatus` from CIP APIs; FreeRTOS create failure logged; cyclic errors set `g_end_stack`.

## Thread safety

Init mutex prevents double start. Application callbacks run in OpENer task context.

## Memory / resources

Connection list + CIP heap via `CipCalloc`/`CipFree` (stdlib). Stack 8192 chosen to avoid overflow (comment in source).

## Limits

Defined by `opener_user_conf.h` connection counts; product uses assemblies 100/150/151.

## Failure modes

| Symptom | Likely cause |
|---------|--------------|
| 16#0315 Invalid segment | Connection path missing config assembly 151 |
| 16#0204 Path destination unknown | EDS/path points at missing instance |
| OpENer not started | Link down at init time |

## Data formats

CIP implicit I/O assemblies — see [assembly_layout](../../artifacts/data/assembly_layout.md) (canonical in P01).

## Integration points

Started from P03 after GOT_IP. Application object in P01. NV store via L04.

## Logging / diagnostics

`OPENER_TRACE_*` macros; ESP_LOG in application layer.

## Portability

ESP32 FreeRTOS + lwIP. Other OpENer ports exist upstream but are not used here.

## Security considerations

No authentication on EtherNet/IP or HTTP config UI — proof-of-concept only.

## Related components

- [nvtcpip](../nvtcpip/README.md)
- [kc868-a16-application](../../project/subsystems/kc868-a16-application/README.md)
- [ethernet-bringup](../../project/subsystems/ethernet-bringup/README.md)

## Source evidence

| Claim | Evidence | Level |
|-------|----------|-------|
| Stack size 8192, prio 5, core 0 | `opener.c` L23–24, L128–134 | E1 |
| NvTcpipLoad during init | `opener.c` L108–113 | E1 |
| CreateAssemblyObject API | `opener_api.h` L593–595 | E1 |
