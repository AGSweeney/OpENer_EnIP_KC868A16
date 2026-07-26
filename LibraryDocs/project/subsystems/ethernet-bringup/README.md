---
title: ethernet-bringup
component: ethernet-bringup
level: project
platforms:
  - ESP32-KC868-A16
topics:
  - lan8720
  - ethernet
  - dhcp
  - startup
source_paths:
  - main/main.c
status: verified
retrieval:
  questions:
    - What is the firmware startup order?
    - When is OpENer started relative to getting an IP?
    - Which PHY and RMII pins does the board use?
  related:
    - ../kc868-a16-application/README.md
    - ../webui/README.md
    - ../../architecture/system-overview.md
---

# ethernet-bringup

`app_main` Ethernet + netif bring-up for the KC868-A16 LAN8720.

## Purpose

Initialize NVS, create ETH netif, apply saved DHCP/static config, start PHY, and on `IP_EVENT_ETH_GOT_IP` start OpENer then WebUI.

## Startup order

1. `nvs_flash_init`
2. `esp_netif` + default event loop
3. `NvTcpipLoad` → decide DHCP vs static
4. Install LAN8720 MAC/PHY (MDC 23, MDIO 18, RMII clock out GPIO 17)
5. Attach netif, set hostname, start DHCP or static
6. `esp_eth_start`
7. On GOT_IP: `opener_init` then `webui_init` ([A-P03-pat](../../../artifacts/patterns/got_ip_start_stack.c))

## Ownership

Runs in `app_main` / ESP event handlers. Idle loop delays 1 s forever after start.

## Source evidence

| Claim | Evidence | Level |
|-------|----------|-------|
| GOT_IP starts OpENer then WebUI | `main.c` L95–101 | E1 |
| PHY MDC 23 / MDIO 18 | `main.c` L48–50 | E1 |
| Early NvTcpipLoad for IP mode | `main.c` L123–141 | E1 |
