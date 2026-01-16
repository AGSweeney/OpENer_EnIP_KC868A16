# Kincony A16 Pin Config Reference

Source: `pinconfig.txt`

## Board + Framework

| Item | Value |
| --- | --- |
| Device name | kc868-a16 |
| MCU | esp32 |
| Framework | esp-idf |

## Ethernet (LAN8720A)

| Signal | GPIO |
| --- | --- |
| MDC | GPIO23 |
| MDIO | GPIO18 |
| TXD0 | GPIO19 |
| TXD1 | GPIO22 |
| TX_EN | GPIO21 |
| RXD0 | GPIO25 |
| RXD1 | GPIO26 |
| RX_DV | GPIO27 |
| REF CLK | GPIO17 |
| PHY addr | 1 |

RST_NET is tied to 3V3 via RC (no ESP32 GPIO).

## UART (RS485)

| Signal | GPIO | Baud |
| --- | --- | --- |
| TX | GPIO13 | 9600 |
| RX | GPIO16 | 9600 |

## Infrared

| Function | GPIO |
| --- | --- |
| IR receiver | GPIO2 |
| IR transmitter | GPIO15 |

## I2C

| Signal | GPIO |
| --- | --- |
| SDA | GPIO4 |
| SCL | GPIO5 |

## PCF8574 I/O Expanders (I2C)

| Role | I2C address | ID |
| --- | --- | --- |
| Inputs X01–X08 | 0x22 | inputs_1_8 |
| Inputs X09–X16 | 0x21 | inputs_9_16 |
| Outputs Y01–Y08 | 0x24 | outputs_1_8 |
| Outputs Y09–Y16 | 0x25 | outputs_9_16 |

## Local GPIO Inputs

| Name | GPIO | Inverted |
| --- | --- | --- |
| KC868-A16-HT1 | GPIO32 | true |
| KC868-A16-HT2 | GPIO33 | true |
| KC868-A16-HT3 | GPIO14 | true |

## Inputs via PCF8574

| Name | Expander | Pin | Inverted |
| --- | --- | --- | --- |
| KC868-A16-X01 | inputs_1_8 | 0 | true |
| KC868-A16-X02 | inputs_1_8 | 1 | true |
| KC868-A16-X03 | inputs_1_8 | 2 | true |
| KC868-A16-X04 | inputs_1_8 | 3 | true |
| KC868-A16-X05 | inputs_1_8 | 4 | true |
| KC868-A16-X06 | inputs_1_8 | 5 | true |
| KC868-A16-X07 | inputs_1_8 | 6 | true |
| KC868-A16-X08 | inputs_1_8 | 7 | true |
| KC868-A16-X09 | inputs_9_16 | 0 | true |
| KC868-A16-X10 | inputs_9_16 | 1 | true |
| KC868-A16-X11 | inputs_9_16 | 2 | true |
| KC868-A16-X12 | inputs_9_16 | 3 | true |
| KC868-A16-X13 | inputs_9_16 | 4 | true |
| KC868-A16-X14 | inputs_9_16 | 5 | true |
| KC868-A16-X15 | inputs_9_16 | 6 | true |
| KC868-A16-X16 | inputs_9_16 | 7 | true |

## Outputs via PCF8574

| Name | Expander | Pin | Inverted |
| --- | --- | --- | --- |
| KC868-A16-Y01 | outputs_1_8 | 0 | true |
| KC868-A16-Y02 | outputs_1_8 | 1 | true |
| KC868-A16-Y03 | outputs_1_8 | 2 | true |
| KC868-A16-Y04 | outputs_1_8 | 3 | true |
| KC868-A16-Y05 | outputs_1_8 | 4 | true |
| KC868-A16-Y06 | outputs_1_8 | 5 | true |
| KC868-A16-Y07 | outputs_1_8 | 6 | true |
| KC868-A16-Y08 | outputs_1_8 | 7 | true |
| KC868-A16-Y09 | outputs_9_16 | 0 | true |
| KC868-A16-Y10 | outputs_9_16 | 1 | true |
| KC868-A16-Y11 | outputs_9_16 | 2 | true |
| KC868-A16-Y12 | outputs_9_16 | 3 | true |
| KC868-A16-Y13 | outputs_9_16 | 4 | true |
| KC868-A16-Y14 | outputs_9_16 | 5 | true |
| KC868-A16-Y15 | outputs_9_16 | 6 | true |
| KC868-A16-Y16 | outputs_9_16 | 7 | true |
