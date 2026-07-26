---
title: System overview
component: system-overview
level: project
platforms:
  - ESP32-KC868-A16
topics:
  - architecture
  - startup
  - data-flow
  - ethernet-ip
source_paths:
  - main/main.c
  - components/opener/src/ports/ESP32/opener.c
  - components/opener/src/ports/ESP32/kc868_a16_application/kc868_a16_application.c
status: verified
retrieval:
  questions:
    - What is the startup order?
    - What are the data flows between subsystems?
    - Where is configuration persisted?
  related:
    - ../subsystems/ethernet-bringup/README.md
    - ../subsystems/kc868-a16-application/README.md
    - ../subsystems/webui/README.md
---

# System overview

KC868-A16 EtherNet/IP adapter: ESP32 + LAN8720 + PCF8574 I/O + OpENer CIP stack.

## Startup order

1. **P03 ethernet-bringup** — NVS, eth driver, IP
2. **L03 opener-esp32** — CIP stack + OpENer task (Core 0)
3. **P01 kc868-a16-application** — assemblies via `ApplicationInitialization` (called from stack init)
4. **P02 webui** — HTTP after OpENer init

## Data flows

```text
PLC scanner --UDP Class1--> OpENer task --assembly 150--> PCF8574 relays
PLC scanner <--assembly 100-- OpENer task <-- PCF8574 inputs + ADC
Browser --HTTP /api/ipconfig--> WebUI --mutex--> g_tcpip --NvTcpipStore--> NVS
```

## Configuration persistence

TCP/IP object → NVS namespace `opener` key `tcpip_cfg` (L04). Identity compile-time in `devicedata.h` (PL03).

## Subsystems

| ID | Role |
|----|------|
| P03 | Eth/netif lifecycle |
| P01 | I/O assemblies |
| P02 | HTTP config UI |

## Source evidence

| Claim | Evidence | Level |
|-------|----------|-------|
| GOT_IP order OpENer then WebUI | `main.c` L95–101 | E1 |
| OpENer task Core 0 | `opener.c` L128–134 | E1 |
| Assemblies created in ApplicationInitialization | `kc868_a16_application.c` L295–320 | E1 |
