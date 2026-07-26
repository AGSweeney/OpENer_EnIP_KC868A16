---
title: Artifacts registry
level: project
status: verified
---

# Artifacts

| ID | File | Component | Usefulness | Description |
|----|------|-----------|------------|-------------|
| A-L01-if | [interfaces/i2c_manager.h](interfaces/i2c_manager.h) | L01 i2c-manager | U1–U6 | Public I2C bus manager API |
| A-L02-if | [interfaces/pcf8574.h](interfaces/pcf8574.h) | L02 pcf8574 | U1–U6 | PCF8574 driver API |
| A-L02-pat | [patterns/pcf8574_requires_i2c_manager.c](patterns/pcf8574_requires_i2c_manager.c) | L02 | U2 | Must init i2c_manager first |
| A-L03-if | [interfaces/opener_api_assembly.h](interfaces/opener_api_assembly.h) | L03 opener-esp32 | U1–U6 | Assembly + connection-point API |
| A-L03-pat | [patterns/opener_init_task.c](patterns/opener_init_task.c) | L03 | U2 | Core-0 pinned OpENer task init |
| A-L04-if | [interfaces/nvtcpip.h](interfaces/nvtcpip.h) | L04 nvtcpip | U1–U6 | Load/store TCP/IP NV API |
| A-L04-data | [data/tcpip_nv_blob.md](data/tcpip_nv_blob.md) | L04 | U2 | Packed NVS blob schema |
| A-P01-pat | [patterns/assembly_init.c](patterns/assembly_init.c) | P01 | U2 | Assemblies 100/150/151 wiring |
| A-P01-data | [data/assembly_layout.md](data/assembly_layout.md) | P01 | U2 | I/O byte map |
| A-P02-data | [data/ipconfig.http](data/ipconfig.http) | P02 | U1 | REST examples |
| A-P02-pat | [patterns/webui_tcpip_mutex.c](patterns/webui_tcpip_mutex.c) | P02 | U2 | Mutex around g_tcpip |
| A-P03-pat | [patterns/got_ip_start_stack.c](patterns/got_ip_start_stack.c) | P03 | U2 | Startup order after GOT_IP |
| A-PL01-bld | [build/cmake_fd_setsize.cmake](build/cmake_fd_setsize.cmake) | PL01 | U2 | FD_SETSIZE / MINIMAL_BUILD |
| A-PL03-data | [data/eds_connection_path.md](data/eds_connection_path.md) | PL03 | U2 | EDS paths vs firmware |

## Bench

No retained bench logs yet. See `project/OPEN_QUESTIONS.md`.
