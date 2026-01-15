# Musical Robotics Class - ESP-NOW Wireless Control

This repository contains ESP32 projects developed for the Musical Robotics course, focusing on wireless communication using ESP-NOW protocol for real-time control applications.

## Project Overview

**ESP-NOW Wireless LED Control System** - A two-board ESP32 system that demonstrates wireless communication for musical robotics applications. The system uses ESP-NOW protocol to send control commands from a transmitter (controlled via Max/MSP) to a receiver, enabling wireless control of actuators, LEDs, or other devices.

## Features

- **Wireless Communication**: Uses ESP-NOW for low-latency, WiFi-based communication (no router needed)
- **Max/MSP Integration**: Serial protocol for easy integration with Max/MSP patches
- **Real-time Control**: Minimal latency for musical and performance applications
- **Bidirectional Feedback**: Status confirmation and delivery acknowledgment
- **Debug Output**: Comprehensive serial debugging on both transmitter and receiver

## Hardware Requirements

- 2x ESP32 development boards (ESP32-D0WD-V3 or compatible)
- USB cables for programming and serial communication
- Built-in LEDs (GPIO2) for testing

## Software Requirements

- Arduino IDE or Arduino CLI
- ESP32 board support (v3.1.1 or higher)
- Max/MSP (for serial control)
- Python 3 with pyserial (for programming)

## Project Structure

```
MUROB_class/
├── espnow_receiver/
│   └── espnow_receiver.ino    # Receiver ESP32 code
├── espnow_transmitter/
│   └── espnow_transmitter.ino # Transmitter ESP32 code (Max/MSP compatible)
└── README.md                   # This file
```

## Quick Start Guide

### 1. Upload Receiver Code

```bash
# Navigate to project directory
cd MUROB_class

# Compile and upload receiver
arduino-cli compile --fqbn esp32:esp32:esp32 espnow_receiver
arduino-cli upload -p /dev/cu.usbserial-XXXX --fqbn esp32:esp32:esp32 espnow_receiver
```

### 2. Get Receiver MAC Address

Open Serial Monitor at 115200 baud and copy the MAC address displayed (format: `80:F3:DA:65:70:CC`)

### 3. Configure Transmitter

Edit `espnow_transmitter/espnow_transmitter.ino` line 27:
```cpp
uint8_t receiverAddress[] = {0x80, 0xF3, 0xDA, 0x65, 0x70, 0xCC};  // Your receiver's MAC
```

### 4. Upload Transmitter Code

```bash
arduino-cli compile --fqbn esp32:esp32:esp32 espnow_transmitter
arduino-cli upload -p /dev/cu.usbserial-YYYY --fqbn esp32:esp32:esp32 espnow_transmitter
```

### 5. Test System

Connect to transmitter's serial port at 115200 baud and send:
- `1` - Turn LED ON
- `0` - Turn LED OFF

## Max/MSP Integration

### Serial Object Configuration

```
Port: /dev/cu.usbserial-XXXX
Baud Rate: 115200
```

### Example Max Patch

```
[toggle]
|
[select 0 1]
|         |
[0(       [1(
|         |
[serial /dev/cu.usbserial-XXXX 115200]
```

### Sending Commands from Max

- Connect a `[serial]` object to the transmitter's port
- Send ASCII character `'1'` (decimal 49) to turn LED ON
- Send ASCII character `'0'` (decimal 48) to turn LED OFF
- The transmitter will relay commands wirelessly to the receiver

## Serial Protocol

### Transmitter Input (from Max/MSP)
| Command | Description |
|---------|-------------|
| `1`     | Turn LED ON and send wireless command |
| `0`     | Turn LED OFF and send wireless command |

### Transmitter Output (debug)
```
[1.5s] Command: LED ON  | TX: Sent
Last Packet Send Status: Delivery Success
```

### Receiver Output (debug)
```
[1.5s] RX #1 | LED: ON  | From: 80:F3:DA:65:67:F4 | Size: 5 bytes
```

## System Architecture

```
Max/MSP → Serial → Transmitter ESP32 → ESP-NOW → Receiver ESP32 → LED/Actuator
                        ↓                           ↓
                   Local LED                  Wireless Control
```

## Technical Details

### ESP-NOW Protocol
- Direct peer-to-peer communication
- No WiFi router required
- ~10ms latency
- 250 byte maximum payload
- 2.4GHz frequency band

### Data Structure
```cpp
typedef struct struct_message {
  bool ledState;           // LED on/off command
  unsigned long timestamp; // For latency measurement
} struct_message;
```

### Power Requirements
- Operating voltage: 3.3V
- Current draw: ~80mA (WiFi active)
- USB powered during development

## Troubleshooting

### Issue: Upload fails with "termios error"
**Solution**: Unplug and replug the ESP32 board, then retry upload.

### Issue: Receiver not responding to commands
**Solution**:
1. Verify MAC address is correct in transmitter code
2. Check both boards are powered on
3. Ensure receiver is in range (~100m line of sight)

### Issue: Max/MSP can't find serial port
**Solution**:
1. Check port name with `ls /dev/cu.usb*`
2. Close Arduino Serial Monitor if open
3. Verify baud rate is set to 115200

### Issue: Garbled serial output
**Solution**: Ensure baud rate is exactly 115200 on both sides.

## Extending the System

### Adding More Receivers
ESP-NOW supports up to 20 peers. Add additional receivers by:
1. Uploading receiver code to new ESP32s
2. Collecting their MAC addresses
3. Adding peers in transmitter's `setup()` function

### Controlling Multiple Parameters
Modify the `struct_message` to include additional data:
```cpp
typedef struct struct_message {
  bool ledState;
  int motorSpeed;        // 0-255
  int servoAngle;        // 0-180
  unsigned long timestamp;
} struct_message;
```

### Bidirectional Communication
Add a send callback on the receiver to acknowledge commands or send sensor data back to transmitter.

## Musical Applications

- **Wireless DMX Controller**: Control stage lighting wirelessly
- **Remote Instrument Triggering**: Trigger percussion or samples remotely
- **Distributed Sound Installation**: Synchronize multiple sound modules
- **Gestural Control**: Wireless sensor data transmission for performance
- **Multi-channel Audio Routing**: Coordinate multiple audio devices

## Course Information

**Course**: Musical Robotics
**Institution**: [Your Institution]
**Instructor**: [Instructor Name]
**Semester**: [Semester/Year]

## License

This project is for educational purposes as part of the Musical Robotics course.

## Contributing

Students are encouraged to fork this repository and experiment with:
- Adding sensors (accelerometers, buttons, potentiometers)
- Implementing OSC protocol for network communication
- Creating Max/MSP patches for musical control
- Building custom actuators and instruments

## Resources

- [ESP-NOW Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/network/esp_now.html)
- [Arduino ESP32 Core](https://github.com/espressif/arduino-esp32)
- [Max/MSP Serial Object](https://docs.cycling74.com/max8/refpages/serial)

## Authors

- **Musical Robotics Class** - Initial implementation
- **Daniel Belquer** - Course participant

## Acknowledgments

- ESP32 community for ESP-NOW examples
- Cycling '74 for Max/MSP documentation
- Musical Robotics course staff

---

**Last Updated**: January 2026
**Version**: 1.0
