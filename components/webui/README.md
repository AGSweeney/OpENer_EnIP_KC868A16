# Web UI Component

A minimal web-based user interface for the ESP32-P4 OpENer EtherNet/IP adapter, providing essential device management capabilities.

## Overview

The Web UI component provides a lightweight, responsive web interface accessible via HTTP on port 80. It focuses on essential functions: network configuration and firmware updates. All other device configuration, monitoring, and status information is available via the REST API.

## Features

- **Network Configuration**: Configure DHCP/Static IP, netmask, gateway, and DNS settings
- **OTA Firmware Updates**: Upload and install firmware updates via web interface
- **REST API**: All sensor configuration, monitoring, and advanced features available via API endpoints
- **Responsive Design**: Works on desktop and mobile devices
- **No External Dependencies**: All CSS and JavaScript is self-contained (no CDN)

## Web Pages

### Configuration Page (`/`)
The main configuration page provides essential device management:

- **Network Configuration Card**
  - DHCP/Static IP mode selection
  - IP address, netmask, gateway configuration
  - DNS server configuration (hidden when using DHCP)
  - All settings stored in OpENer's NVS
  - Reboot required to apply network changes

### Firmware Update Page (`/ota`)
Over-the-air firmware update interface:

- File upload for firmware binary
- Progress indication
- Auto-redirect to home page after successful update
- Styled file input button matching application design

**Note:** All other device configuration, sensor monitoring, assembly data viewing, and advanced features are available via the REST API. See [docs/API_Endpoints.md](../../docs/API_Endpoints.md) for complete API documentation.

## REST API Endpoints

All API endpoints return JSON responses.

### Configuration Endpoints

#### `GET /api/mpu6050/status`
Get current MPU6050 sensor status and readings.

**Response:**
```json
{
  "roll": 12.34,
  "pitch": -5.67,
  "ground_angle": 13.45,
  "bottom_pressure": 123.4,
  "top_pressure": 876.6,
  "enabled": true,
  "byte_offset": 0
}
```

#### `GET /api/lsm6ds3/status`
Get current LSM6DS3 sensor status and readings (same format as MPU6050).

### Status Endpoints

#### `GET /api/assemblies/sizes`
Get EtherNet/IP assembly sizes.

**Response:**
```json
{
  "input_assembly_size": 32,
  "output_assembly_size": 32
}
```

#### `GET /api/status`
Get assembly data for status pages (includes input and output assembly raw bytes).

**Response:**
```json
{
  "input_assembly_100": {
    "raw_bytes": [0, 1, 2, ...]
  },
  "output_assembly_150": {
    "raw_bytes": [0, 0, 0, ...]
  }
}
```

### Network Configuration Endpoints

#### `GET /api/ipconfig`
Get current IP configuration.

**Response:**
```json
{
  "use_dhcp": true,
  "ip_address": "192.168.1.100",
  "netmask": "255.255.255.0",
  "gateway": "192.168.1.1",
  "dns1": "8.8.8.8",
  "dns2": "8.8.4.4"
}
```

#### `POST /api/ipconfig`
Update IP configuration.

**Request Body:**
```json
{
  "use_dhcp": false,
  "ip_address": "192.168.1.100",
  "netmask": "255.255.255.0",
  "gateway": "192.168.1.1",
  "dns1": "8.8.8.8",
  "dns2": "8.8.4.4"
}
```

**Response:**
```json
{
  "status": "ok",
  "message": "IP configuration saved successfully. Reboot required to apply changes."
}
```

### Modbus Configuration Endpoints

#### `GET /api/modbus`
Get Modbus TCP enabled state.

**Response:**
```json
{
  "enabled": true
}
```

#### `POST /api/modbus`
Set Modbus TCP enabled state.

**Request Body:**
```json
{
  "enabled": true
}
```

**Response:**
```json
{
  "status": "ok",
  "enabled": true,
  "message": "Modbus configuration saved successfully"
}
```

### Sensor Control Endpoints

#### `GET /api/mpu6050/enabled` and `GET /api/lsm6ds3/enabled`
Get sensor enabled state.

**Response:**
```json
{
  "enabled": true
}
```

#### `POST /api/mpu6050/enabled` and `POST /api/lsm6ds3/enabled`
Enable or disable the sensor.

**Request Body:**
```json
{
  "enabled": true
}
```

**Response:**
```json
{
  "status": "ok",
  "enabled": true,
  "message": "Sensor state saved successfully"
}
```

#### `GET /api/mpu6050/byteoffset` and `GET /api/lsm6ds3/byteoffset`
Get sensor data byte offset in Input Assembly 100.

**Response:**
```json
{
  "byte_offset": 0
}
```

#### `POST /api/mpu6050/byteoffset` and `POST /api/lsm6ds3/byteoffset`
Set sensor data byte offset (0-12, must leave 20 bytes free).

**Request Body:**
```json
{
  "byte_offset": 0
}
```

#### `GET /api/lsm6ds3/calibrate`
Get LSM6DS3 calibration status.

**Response:**
```json
{
  "calibrated": true,
  "gyro_offset_mdps": [1.2, -0.5, 0.8]
}
```

#### `POST /api/lsm6ds3/calibrate`
Trigger LSM6DS3 calibration.

**Request Body (optional):**
```json
{
  "samples": 100,
  "sample_delay_ms": 10
}
```

**Response:**
```json
{
  "status": "ok",
  "message": "Calibration started"
}
```

#### `GET /api/mpu6050/toolweight`, `GET /api/mpu6050/tipforce`, `GET /api/mpu6050/cylinderbore`
Get tool weight, tip force, or cylinder bore configuration.

**Response:**
```json
{
  "tool_weight": 50
}
```

#### `POST /api/mpu6050/toolweight`, `POST /api/mpu6050/tipforce`, `POST /api/mpu6050/cylinderbore`
Set tool weight (0-255 lbs), tip force (0-255 lbs), or cylinder bore (0-255, scaled by 100: 0.01-2.55 inches).


### OTA Endpoints

#### `POST /api/ota/update`
Trigger OTA firmware update.

**Request:** Multipart form data with firmware file, or JSON with URL:
```json
{
  "url": "http://example.com/firmware.bin"
}
```

**Response:**
```json
{
  "status": "ok",
  "message": "OTA update started"
}
```

#### `GET /api/ota/status`
Get OTA update status.

**Response:**
```json
{
  "status": "idle",
  "progress": 0
}
```

### System Endpoints

#### `GET /api/logs`
Get system logs from circular buffer.

**Response:**
```json
{
  "logs": "Log entry 1\nLog entry 2\n...",
  "size": 1024
}
```

#### `GET /api/i2c/pullup`
Get I2C pull-up enabled state.

**Response:**
```json
{
  "enabled": true
}
```

#### `POST /api/i2c/pullup`
Set I2C pull-up enabled state.

**Request Body:**
```json
{
  "enabled": true
}
```

#### `POST /api/reboot`
Reboot the device.

**Response:**
```json
{
  "status": "ok",
  "message": "Device will reboot in 2 seconds"
}
```

## Architecture

### Components

- **`webui.c`**: HTTP server initialization and page routing
- **`webui_html.c`**: HTML, CSS, and JavaScript for all web pages (embedded as C strings)
- **`webui_api.c`**: REST API endpoint handlers

### HTTP Server Configuration

- **Port**: 80
- **Max URI Handlers**: 35 (currently 34 handlers: 4 HTML pages + 30 API endpoints)
- **Max Open Sockets**: 7
- **Stack Size**: 20KB (increased for large HTML pages and file uploads)
- **Task Priority**: 5
- **Max Request Header Length**: 1024 bytes

### Data Storage

- **Network Configuration**: Stored in OpENer's `g_tcpip` NVS namespace
- **Modbus Configuration**: Stored in `system` NVS namespace
- **Sensor Enabled State**: Stored in `system` NVS namespace

### Sensor Data Mapping

IMU sensor data (MPU6050 or LSM6DS3) is written to Input Assembly 100 (`g_assembly_data064`) as 5 DINTs (20 bytes) at a configurable byte offset:

- **DINT 0**: Roll angle (degrees × 10000, little-endian)
- **DINT 1**: Pitch angle (degrees × 10000, little-endian)
- **DINT 2**: Ground angle from vertical (degrees × 10000, little-endian)
- **DINT 3**: Bottom cylinder pressure (PSI × 1000, little-endian)
- **DINT 4**: Top cylinder pressure (PSI × 1000, little-endian)

When the sensor is disabled, these bytes are zeroed out. See [docs/ASSEMBLY_DATA_LAYOUT.md](../../docs/ASSEMBLY_DATA_LAYOUT.md) for complete details.

## Usage

### Accessing the Web Interface

1. Ensure the ESP32-P4 device is powered on and connected to your network
2. Find the device's IP address (check serial monitor or DHCP server)
3. Open a web browser and navigate to `http://<device-ip>`
4. The Configuration page will load automatically

### Configuration Workflow

1. **Network Setup** (if needed):
   - Navigate to Configuration page (`/`)
   - Configure IP settings in Network Configuration card
   - Click "Save Network Configuration"
   - Reboot device to apply changes

2. **Sensor Configuration and Monitoring**:
   - Use REST API endpoints (see [docs/API_Endpoints.md](../../docs/API_Endpoints.md))
   - Examples: `/api/mpu6050/status`, `/api/lsm6ds3/status`, `/api/assemblies`

### Firmware Update

1. Navigate to Firmware Update page (`/ota`)
2. Click "Choose File" and select firmware binary
3. Click "Start Update"
4. Wait for update to complete
5. Device will automatically reboot
6. Browser will redirect to home page

## Development

### Adding a New Page

1. Add HTML function in `webui_html.c`:
   ```c
   const char *webui_get_newpage_html(void)
   {
       return "<!DOCTYPE html>..."
   }
   ```

2. Register URI handler in `webui.c`:
   ```c
   httpd_uri_t newpage_uri = {
       .uri = "/newpage",
       .method = HTTP_GET,
       .handler = newpage_handler,
       .user_ctx = NULL
   };
   httpd_register_uri_handler(server_handle, &newpage_uri);
   ```

3. Implement handler function that calls the HTML getter

### Adding a New API Endpoint

1. Implement handler function in `webui_api.c`:
   ```c
   static esp_err_t api_get_newendpoint_handler(httpd_req_t *req)
   {
       // Implementation
   }
   ```

2. Register in `webui_register_api_handlers()`:
   ```c
   httpd_uri_t get_newendpoint_uri = {
       .uri = "/api/newendpoint",
       .method = HTTP_GET,
       .handler = api_get_newendpoint_handler,
       .user_ctx = NULL
   };
   httpd_register_uri_handler(server, &get_newendpoint_uri);
   ```

### Updating Preview Files

Preview HTML files in `webui_preview/` can be updated using the extraction script:

```bash
python update_webui_previews.py
```

This script extracts HTML from `webui_html.c` and updates all preview files.

## Notes

- All settings persist across reboots via NVS
- Network configuration changes require a reboot to take effect
- The web UI has no external dependencies (no CDN, all assets embedded)
- All sensor configuration, monitoring, and advanced features are available via REST API
- See [docs/API_Endpoints.md](../../docs/API_Endpoints.md) for complete API documentation

## Footer

All pages display the footer:
```
OpENer Ethernet/IP for ESP32-P4 | Adam G Sweeney
```

