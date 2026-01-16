# EtherNet/IP Controller for Kincony KC868-A16

## Overview

This project represents a proof-of-concept implementation of the [OpENer EtherNet/IP Stack](https://github.com/EIPStackGroup/OpENer) ported to the ESP32 platform. The development of this port serves to demonstrate the feasibility of running industrial EtherNet/IP communication protocols on cost-effective, widely available microcontroller platforms.

This implementation targets the **Kincony KC868-A16** Ethernet I/O controller, exposing its 16 relay outputs, 16 digital inputs (via PCF8574), and 4 analog inputs over EtherNet/IP implicit messaging. The result is a compact, network-connected I/O device suitable for PLC integration and general industrial automation tasks.

## Project Status

**Proof of Concept**: This project is a demonstration and validation of the OpENer EtherNet/IP stack on ESP32 hardware. It is not intended for production use without further development, testing, and validation.

## Target Hardware

### Kincony KC868-A16

The Kincony KC868-A16 is an ESP32-based Ethernet I/O controller with 16 relay outputs, 16 opto-isolated inputs provided by PCF8574 I/O expanders, and 4 analog inputs. This project uses the board's LAN8720 Ethernet PHY, PCF8574 expanders, and built-in ADC to expose I/O over EtherNet/IP.

**Key Features:**
- **Microcontroller**: ESP32
- **Ethernet**: LAN8720 PHY with RMII interface
- **Digital Outputs**: 16 relay outputs via PCF8574 (active-low)
- **Digital Inputs**: 16 opto-isolated inputs via PCF8574 (active-low)
- **Analog Inputs**: 4 channels (2x 4-20mA, 2x 0-5V) via ESP32 ADC
- **I2C**: SDA GPIO4, SCL GPIO5 for PCF8574 expanders (400 kHz)

**Reference Material:**
- `Notes/pinconfig.md`
- `Notes/pinconfig.txt`
- `Notes/KC868-A16-schematic.pdf`

## OpENer EtherNet/IP Stack

This project integrates the [OpENer EtherNet/IP Stack](https://github.com/EIPStackGroup/OpENer), an open-source implementation of the Common Industrial Protocol (CIP) over Ethernet. OpENer provides:

- **EtherNet/IP Explicit Messaging**: For configuration and data exchange
- **EtherNet/IP Implicit Messaging**: For real-time I/O connections
- **CIP Objects**: Identity, TCP/IP Interface, Ethernet Link, Assembly, Connection Manager, QoS
- **NVS Storage**: Persistent storage of network configuration (IP address, hostname, etc.)

## Assembly Data Structure

The device implements two EtherNet/IP assemblies for I/O data exchange. These assemblies define the data structure used for communication between the EtherNet/IP scanner (typically a PLC) and the device.

### Output Assembly (Instance 150) - 2 Bytes

The output assembly is used to send relay control data from the EtherNet/IP scanner (PLC) to the device.

| Byte | Bits | Outputs | Description |
|------|------|---------|-------------|
| 0 | 0-7 | Y01-Y08 | Relay outputs 1-8 |
| 1 | 0-7 | Y09-Y16 | Relay outputs 9-16 |

**Implementation Notes:**
- Bit value 1 = relay on, 0 = relay off.
- Outputs are active-low at the PCF8574, so the firmware inverts bits when writing to the expander.

### Input Assembly (Instance 100) - 10 Bytes

The input assembly is used to send digital input states and analog readings from the device back to the EtherNet/IP scanner.

| Offset | Size | Inputs | Description |
|------|------|--------|-------------|
| 0 | 1 | X01-X08 | Opto inputs 1-8 |
| 1 | 1 | X09-X16 | Opto inputs 9-16 |
| 2 | 2 | A1 (INA1) | 4-20 mA raw ADC count (little-endian) |
| 4 | 2 | A2 (INA2) | 0-5 V raw ADC count (little-endian) |
| 6 | 2 | A3 (INA3) | 0-5 V raw ADC count (little-endian) |
| 8 | 2 | A4 (INA4) | 4-20 mA raw ADC count (little-endian) |

**Implementation Notes:**
- Digital inputs are active-low; the firmware reports a bit as 1 when the physical signal is low.
- Analog inputs are sampled at 12-bit resolution (0-4095 counts) with 11 dB attenuation.
- Analog input mapping: A1 (INA1) and A4 (INA4) are 4-20mA inputs; A2 (INA2) and A3 (INA3) are 0-5V inputs.

## GPIO Pin Assignments

### I2C (PCF8574)

| Signal | GPIO |
|--------|------|
| SDA | GPIO 4 |
| SCL | GPIO 5 |

### PCF8574 I/O Expanders

| Role | I2C Address |
|------|-------------|
| Inputs X01-X08 | 0x22 |
| Inputs X09-X16 | 0x21 |
| Outputs Y01-Y08 | 0x24 |
| Outputs Y09-Y16 | 0x25 |

### Ethernet Configuration

Ethernet connectivity is provided via the LAN8720 PHY:

| Signal | GPIO | Function |
|--------|------|----------|
| MDC | GPIO 23 | Ethernet management clock |
| MDIO | GPIO 18 | Ethernet management data |
| TXD0 | GPIO 19 | RMII TXD0 |
| TXD1 | GPIO 22 | RMII TXD1 |
| TX_EN | GPIO 21 | RMII TX_EN |
| RXD0 | GPIO 25 | RMII RXD0 |
| RXD1 | GPIO 26 | RMII RXD1 |
| RX_DV | GPIO 27 | RMII CRS_DV |
| REF CLK | GPIO 17 | RMII clock output |

**Ethernet PHY**: LAN8720 (address 0)

## EtherNet/IP Connection Types

The device supports three standard EtherNet/IP connection types, providing flexibility for different integration scenarios:

1. **Exclusive Owner** (Connection 1)
   - Bidirectional: Output 2 bytes, Input 10 bytes
   - Full control and status monitoring
   - Configurable RPI (Requested Packet Interval)
   - Recommended for primary control applications

2. **Input Only** (Connection 2)
   - Unidirectional: Input 10 bytes only
   - Status monitoring without control capability
   - Configurable RPI
   - Useful for monitoring-only applications

3. **Listen Only** (Connection 3)
   - Unidirectional: Input 10 bytes only
   - Read-only status monitoring
   - Configurable RPI
   - Suitable for passive monitoring and diagnostics

## Device Identity

The device presents the following identity information to EtherNet/IP scanners:

- **Vendor ID**: 55512 (AGSweeney Automation)
- **Device Type**: 7 (Generic Device)
- **Product Code**: 1
- **Major Revision**: 1
- **Minor Revision**: 1
- **Product Name**: "KC868-A16"
- **Serial Number**: 1234567890

## Network Configuration

The device supports both DHCP and static IP configuration, providing flexibility for different network environments:

- **DHCP**: Automatically obtains IP address, gateway, and DNS from network DHCP server
- **Static IP**: Manually configured IP address, netmask, gateway, and DNS servers
- **Hostname**: Configurable (default: "KC868-A16-EnIP")
- **NVS Storage**: Network configuration is saved to NVS flash and persists across reboots

### Web UI

A minimal web interface is provided for network configuration and diagnostics:

- **URL**: `http://<device-ip>/`
- **API Endpoint**: `/api/ipconfig` (GET/POST)
- **Features**: View and configure IP settings (DHCP/Static, IP address, netmask, gateway, DNS)

The web interface provides a simple means to configure network settings without requiring EtherNet/IP tools or serial console access.

## Building and Flashing

### Prerequisites

- **ESP-IDF**: Version 5.5.1 or later
- **Python**: 3.8 or later
- **CMake**: 3.16 or later

### Build Steps

```bash
# Set up ESP-IDF environment
. $HOME/esp/esp-idf/export.sh

# Configure the project
idf.py menuconfig

# Build the project
idf.py build

# Flash to device
idf.py flash

# Monitor serial output
idf.py monitor
```

### Partition Table

The device uses a 4MB flash with the following partition layout:

- **NVS**: 20 KB (0x9000 - 0xE000) - Non-volatile storage for configuration
- **OTA Data**: 8 KB (0xE000 - 0x10000) - OTA update metadata
- **OTA_0**: 1.5 MB (0x10000 - 0x190000) - Primary application partition
- **OTA_1**: 1.5 MB (0x190000 - 0x310000) - Secondary application partition (for OTA updates)
- **SPIFFS**: 960 KB (0x310000 - 0x400000) - File system partition

## EDS File

**⚠️ WARNING: The EDS file is incomplete and should NOT be used for production or device configuration.**

An Electronic Data Sheet (EDS) file is provided for reference only:

- **Location**: `eds/KC868A16.eds`
- **Format**: EZ-EDS v3.38
- **Status**: Incomplete - Do not use

The EDS file is a work in progress and may contain errors or missing information. It should not be used with EtherNet/IP configuration tools until it has been completed and validated.

## Project Structure

```
KC868_A16_EnIP/
├── main/                    # Main application code
│   └── main.c              # Ethernet and OpENer initialization
├── components/
│   ├── opener/            # OpENer EtherNet/IP stack
│   │   └── src/ports/ESP32/
│   │       └── kc868_a16_application/  # Application-specific I/O code
│   ├── i2c_manager/       # I2C bus management component
│   ├── pcf8574/           # PCF8574 I/O expander driver component
│   ├── webui/             # Web interface for network configuration
│   ├── lwip/              # LWIP network stack
│   └── esp_netif/         # ESP-IDF network interface
├── docs/                   # Documentation and images
├── eds/                    # Electronic Data Sheet (incomplete)
└── partitions.csv         # Flash partition table
```

## License

### Original Code

All original code in this project is licensed under the MIT License:

```
Copyright (c) 2025, Adam G. Sweeney <agsweeney@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
```

### Third-Party Components

This project integrates the following third-party components with their respective licenses:

- **OpENer EtherNet/IP Stack**: Licensed under an adapted BSD-style license by Rockwell Automation, Inc. See `components/opener/LICENSE.txt` for full license terms. EtherNet/IP is a trademark of ODVA, Inc.

- **LWIP**: Licensed under a BSD-style license by Swedish Institute of Computer Science. See `components/lwip/lwip/COPYING` for full license terms.

- **ESP-IDF**: Licensed under Apache License 2.0 by Espressif Systems. See ESP-IDF documentation for full license terms.

All third-party licenses are preserved and remain intact. Please refer to the respective component directories for complete license information.

## Acknowledgments

- **OpENer Stack**: [EIPStackGroup/OpENer](https://github.com/EIPStackGroup/OpENer) - Open-source EtherNet/IP stack
- **ESP-IDF**: Espressif Systems - ESP32 development framework

## Support

For issues, questions, or contributions, please refer to the project repository or contact the maintainer.

---

**Note**: This documentation reflects the current proof-of-concept implementation. For the latest updates and changes, please refer to the project repository.
