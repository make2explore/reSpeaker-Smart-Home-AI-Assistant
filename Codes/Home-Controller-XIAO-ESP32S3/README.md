# Home Controller — XIAO ESP32-S3

The **Home Controller** is one of the three main hardware/software components of our ***reSpeaker Smart Home AI Assistant*** project. It uses a **Seeed Studio XIAO ESP32-S3**, an **SHT40 temperature and humidity sensor**, and a **4-channel relay module**.

This folder contains the Arduino firmware for the Home Controller.

## 1. Requirements

### Hardware
- Seeed Studio XIAO ESP32-S3
- SHT40 temperature and humidity sensor
- 4-channel relay module
- USB data cable
- Jumper/Grove wiring as required

### Software
- Arduino IDE 2.x
- Espressif ESP32 board package
- Sensirion I2C SHT4X Arduino library
- Sensirion Core library dependency

> **Safety:** If using mains-powered loads, use properly rated relays, isolation, fusing, enclosure, and appropriate electrical safety procedures. Low-voltage loads are recommended for testing.

---

## 2. Install Arduino IDE

Download and install Arduino IDE using the official Arduino instructions:

**Official guide:**  
https://support.arduino.cc/hc/en-us/articles/360019833020-Download-and-install-Arduino-IDE

### Windows
1. Download the latest Arduino IDE for Windows.
2. Run the `.exe` installer.
3. Follow the installation wizard.
4. Launch Arduino IDE.

### macOS
1. Download the `.dmg` package.
2. Open it.
3. Drag Arduino IDE to Applications.
4. Launch it.

### Linux
1. Download the appropriate package.
2. Follow Arduino's Linux installation instructions.
3. If using the AppImage, make it executable.
4. Launch Arduino IDE.

---

## 3. Install the ESP32 Board Package

Seeed Studio's official XIAO ESP32-S3 guide:

https://wiki.seeedstudio.com/xiao_esp32s3_getting_started/

### Step 1 — Add the Espressif package URL

In Arduino IDE, open:

**File → Preferences**

Under **Additional Boards Manager URLs**, add:

```text
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
```

If other URLs are already present, add this on a separate line.

### Step 2 — Install the package

Go to:

**Tools → Board → Boards Manager...**

Search for:

```text
esp32
```

Install **esp32 by Espressif Systems**.

Seeed's documentation notes that the XIAO ESP32-S3 requires ESP32 board package version **2.0.8 or newer**.

### Step 3 — Select the board

Connect the XIAO ESP32-S3 with a USB **data** cable.

Select:

**Tools → Board → esp32 → XIAO_ESP32S3**

Then select the serial port belonging to the XIAO.

---

## 4. Install the SHT40 / SHT4x Library

The Home Controller uses the Sensirion SHT40 sensor.

### Official resources

**Sensirion Arduino library:**  
https://github.com/Sensirion/arduino-i2c-sht4x

**Seeed Studio Grove SHT4x guide:**  
https://wiki.seeedstudio.com/Grove-SHT4x/

### Install through Arduino Library Manager

Open:

**Sketch → Include Library → Manage Libraries...**

Search for:

```text
Sensirion I2C SHT4X
```

Install **Sensirion I2C SHT4X**.

Then search for:

```text
Sensirion Core
```

and install the dependency as well.

### ZIP installation

If the library is not available in Library Manager:

1. Download the latest release from the Sensirion repository.
2. In Arduino IDE select:
   **Sketch → Include Library → Add .ZIP Library...**
3. Select the downloaded ZIP.
4. Install the required dependency in the same way if necessary.

The official Sensirion library supports SHT40, SHT41 and SHT45 sensors.

---

## 5. SHT40 Wiring

For the standard XIAO ESP32-S3 I²C pins:

| SHT40 | XIAO ESP32-S3 |
|---|---|
| SDA | D4 / GPIO5 |
| SCL | D5 / GPIO6 |
| VDD | 3.3V |
| GND | GND |

The Sensirion documentation recommends a 3.3 V supply for the SHT4x.

> If you are using a Grove SHT4x carrier, follow the wiring and power requirements of your specific carrier board.

---

## 6. 4-Channel Relay

The four relay channels are used for the four demonstration devices:

| Relay | Demonstration Device |
|---|---|
| Relay 1 | Light |
| Relay 2 | Fan |
| Relay 3 | TV |
| Relay 4 | AC |

**Important:** Use the GPIO definitions in the supplied source code as the authoritative pin assignment. Do not assume GPIO numbers from another project.

---

## 7. Open and Configure the Code

1. Launch Arduino IDE.
2. Open the Home Controller `.ino` file in this folder.
3. Confirm that the required libraries are installed.
4. Select:
   **Tools → Board → esp32 → XIAO_ESP32S3**
5. Select the correct serial port.
6. Review the GPIO/configuration definitions before connecting the relay wiring.

---

## 8. Compile

Click **Verify** or:

**Sketch → Verify/Compile**

Common errors:

### `SensirionI2cSht4x.h: No such file or directory`
Install:

**Sensirion I2C SHT4X**

### Missing Sensirion Core
Install:

**Sensirion Core**

### XIAO ESP32-S3 is missing from the board list
Recheck that **esp32 by Espressif Systems** is installed through Boards Manager.

---

## 9. Upload

1. Connect the XIAO with a USB data cable.
2. Select **XIAO_ESP32S3**.
3. Select the correct port.
4. Click **Upload**.

After a successful upload, the Home Controller firmware will start running on the XIAO.

---

## 10. If the XIAO Is Not Detected

Seeed Studio recommends using Bootloader mode when the XIAO has no serial port or uploads fail.

1. Press and hold **BOOT**.
2. Connect the XIAO to the computer while holding BOOT.
3. Release BOOT after connection.
4. Check the available serial ports.
5. Select the new port in Arduino IDE.
6. Upload again.

Seeed also documents a BOOT + RESET recovery method in its XIAO ESP32-S3 guide.

---

## 11. Test the SHT40 Separately

The Sensirion library provides an example for testing the sensor.

Open:

**File → Examples → Sensirion I2C SHT4X → exampleUsage**

Upload it and open **Serial Monitor**.

The example uses the SHT40 at I²C address **0x44**. The library documentation also lists **0x45** as a supported address for SHT4x devices.

For the example, use:

```text
115200 baud
```

This is a useful way to confirm that the sensor and I²C wiring are working before troubleshooting the complete Home Controller.

---

## 12. Home Controller Architecture

```text
                 ┌─────────────────────────┐
                 │     XIAO ESP32-S3       │
                 │     Home Controller     │
                 └───────────┬─────────────┘
                             │
                ┌────────────┴────────────┐
                │                         │
              I²C                    GPIO Control
                │                         │
                ▼                         ▼
        ┌──────────────┐          ┌────────────────┐
        │    SHT40     │          │ 4-Channel Relay│
        │ Temperature  │          │     Module     │
        │ + Humidity   │          └───────┬────────┘
        └──────────────┘                  │
                                  ┌────────┼────────┐
                                  ▼        ▼        ▼
                               Light     Fan    TV / AC
```

The SHT40 provides environmental measurements, while the relay module provides physical switching for the four controlled devices.

---

## 13. Before Running the Complete System

- [ ] Arduino IDE installed
- [ ] ESP32 board package installed
- [ ] XIAO_ESP32S3 selected
- [ ] Correct serial port selected
- [ ] Sensirion I2C SHT4X installed
- [ ] Sensirion Core installed
- [ ] SHT40 wiring checked
- [ ] Relay GPIO wiring matches the source code
- [ ] USB cable supports data
- [ ] Firmware compiles successfully
- [ ] Firmware uploads successfully
- [ ] SHT40 readings verified
- [ ] Relay operation tested safely

---

## 14. Useful Official Resources

### Arduino
https://support.arduino.cc/hc/en-us/articles/360019833020-Download-and-install-Arduino-IDE

### Seeed Studio — XIAO ESP32-S3
https://wiki.seeedstudio.com/xiao_esp32s3_getting_started/

### Seeed Studio — Grove SHT4x
https://wiki.seeedstudio.com/Grove-SHT4x/

### Sensirion — Arduino SHT4x Library
https://github.com/Sensirion/arduino-i2c-sht4x

---

## 15. Part of the Complete 'reSpeaker Smart Home AI Assistant' Project

This Home Controller is one part of our complete 'reSpeaker Smart Home AI Assistant' system:

- **Seeed Studio reSpeaker Voice AI Development Kit** — the voice interface and hero hardware
- **XIAO ESP32-S3 Home Controller** — device control and environmental sensing
- **CrowPanel Advance 7** — touchscreen dashboard
- **Xiaozhi AI MCP chatbot** — foundation of our voice assistant, Joy

Together, these components provide voice interaction, AI-assisted control, physical device switching, environmental monitoring, and a visual touchscreen interface.
