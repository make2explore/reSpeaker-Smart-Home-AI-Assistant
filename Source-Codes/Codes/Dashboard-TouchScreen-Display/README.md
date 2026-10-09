# ESP-IDF Development Environment — CrowPanel Advance 7

This guide explains how to install and configure **ESP-IDF** and **Visual Studio Code with the official ESP-IDF extension** for the **Elecrow CrowPanel Advance 7-inch HMI ESP32-S3 AI-Powered IPS Touch Screen** used as the Dashboard in our Voice-Controlled Embedded Dashboard project.

The dashboard hardware is an **ESP32-S3-based 7-inch 800 × 480 touchscreen** and uses **LVGL** for the graphical user interface.

> **Project note:** Our existing Dashboard firmware has been developed with **ESP-IDF 5.5.1**. If you are reproducing the firmware from this repository, use the ESP-IDF version required by the project files/environment rather than automatically switching to the newest ESP-IDF release.
>
> Elecrow's current official **CrowPanel Advance 7.0-inch V1.5 ESP-IDF course** uses **ESP-IDF v5.5.4** and LVGL 9.1.0. That is the version used by Elecrow's current course material, not necessarily the version required by an existing project. See the official Elecrow course before changing an established project environment.

---

## 1. About the CrowPanel Advance 7

The **CrowPanel Advance 7.0-inch HMI ESP32-S3** is a touchscreen development platform built around the ESP32-S3.

According to Elecrow's official documentation, the 7-inch version provides:

- 7-inch IPS touchscreen
- 800 × 480 resolution
- ESP32-S3 controller
- SC7277 display driver
- GT911 touch controller
- LVGL support
- ESP-IDF support
- Arduino support
- Audio and peripheral interfaces

Official Elecrow ESP-IDF documentation:

https://www.elecrow.com/wiki/CrowPanel_Advance_7.0inch_ESP_IDF_Course_V15.html

Elecrow's general CrowPanel Advance HMI ESP-IDF page:

https://elecrow.com/wiki/HMI_Display_course_esp_idf.html

---

# 2. ESP-IDF Version — Important for This Project

ESP-IDF versions are not always interchangeable.

An existing ESP-IDF project can depend on a particular:

- ESP-IDF release
- compiler/toolchain
- component version
- LVGL version
- display driver
- Kconfig configuration
- build-system behavior
- API implementation

### Our existing Dashboard project

The Dashboard development environment has been using:

```text
ESP-IDF v5.5.1
```

Therefore, when your goal is to **build the Dashboard firmware from this project**, use:

```text
ESP-IDF v5.5.1
```

unless the project's documentation or configuration has explicitly been updated to another version.

### Elecrow's current official course

Elecrow's current **CrowPanel Advance 7.0-inch V1.5 ESP-IDF Course** specifies:

```text
ESP-IDF v5.5.4
ESP32-S3
800 × 480
LVGL 9.1.0
```

Official course:

https://www.elecrow.com/wiki/CrowPanel_Advance_7.0inch_ESP_IDF_Course_V15.html

This is useful for learning the board and following Elecrow's examples, but it should not be treated as an automatic requirement for an existing project.

---

# 3. What Is ESP-IDF?

**ESP-IDF (Espressif IoT Development Framework)** is Espressif's official development framework for ESP32-family microcontrollers.

For the CrowPanel Dashboard, ESP-IDF provides the underlying development environment for:

- ESP32-S3 firmware
- FreeRTOS
- GPIO
- I²C
- display interfaces
- touch input
- networking
- Wi-Fi
- audio/peripherals
- NVS
- partition tables
- CMake-based builds
- flashing
- serial monitoring
- debugging

The Dashboard application uses **LVGL** for the graphical user interface.

In our project, the Dashboard combines LVGL UI elements with network communication to the Home Controller and displays information such as:

- Day/date
- Temperature
- Humidity
- Home/device status
- Device controls
- Online/offline state

---

# 4. Recommended Development Architecture

```text
                     Development Computer
                              │
              ┌───────────────┴───────────────┐
              │                               │
       Visual Studio Code                 ESP-IDF
              │                           v5.5.1
              │                               │
       ESP-IDF Extension                    │
              │                               │
              └───────────────┬───────────────┘
                              │
                         USB-C Data
                              │
                              ▼
                 ┌────────────────────────┐
                 │   CrowPanel Advance 7  │
                 │       ESP32-S3         │
                 │                        │
                 │       LVGL UI          │
                 │                        │
                 │  800 × 480 Touchscreen │
                 └────────────────────────┘
                              │
                         Wi-Fi / Network
                              │
                              ▼
                     Home Controller
```

---

# 5. Official Resources

These are the primary resources used for this README.

## Espressif

### ESP-IDF Get Started

https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/

### ESP-IDF Installation Manager (EIM)

https://docs.espressif.com/projects/idf-im-ui/en/latest/

### ESP-IDF Installation Methods

https://docs.espressif.com/projects/idf-im-ui/en/latest/installation_methods.html

### EIM CLI Installation

https://docs.espressif.com/projects/idf-im-ui/en/latest/cli_installation.html

### ESP-IDF VS Code Extension

https://docs.espressif.com/projects/vscode-esp-idf-extension/en/latest/

### VS Code Extension Installation

https://docs.espressif.com/projects/vscode-esp-idf-extension/en/latest/installation.html

### ESP-IDF Extension — Create/Open Projects

https://docs.espressif.com/projects/vscode-esp-idf-extension/en/latest/startproject.html

### ESP-IDF Extension — Configure Projects

https://docs.espressif.com/projects/vscode-esp-idf-extension/en/latest/configureproject.html

---

## Elecrow

### CrowPanel Advance HMI ESP-IDF Course

https://elecrow.com/wiki/HMI_Display_course_esp_idf.html

### CrowPanel Advance 7.0-inch ESP-IDF Course V1.5

https://www.elecrow.com/wiki/CrowPanel_Advance_7.0inch_ESP_IDF_Course_V15.html

### CrowPanel Advance 7 ESP32-S3 GitHub Repository

https://github.com/Elecrow-RD/CrowPanel-Advance-7-HMI-ESP32-S3-AI-Powered-IPS-Touch-Screen-800x480

### Elecrow ESP-IDF Installation Guide

https://www.elecrow.com/wiki/ESP_IDF_Installation_Guide.html

### Elecrow Lesson 1 — IDF-IDE Installation and Hello World

https://elecrow.com/wiki/advance_idf_lesson1.html

---

# 6. Install ESP-IDF — Overview

Espressif's current recommended installation workflow uses the:

**ESP-IDF Installation Manager (EIM)**

EIM manages:

- ESP-IDF versions
- compiler/toolchains
- Python environments
- CMake
- Ninja
- flashing tools
- other ESP-IDF development tools

EIM is available for:

- Windows
- Linux
- macOS

It provides both:

- GUI installation
- CLI installation

For most users, the GUI is the easiest method.

For this project, however, make sure you select the required version:

```text
ESP-IDF v5.5.1
```

---

# 7. Windows — Install ESP-IDF

## 7.1 Prerequisites

Espressif's current EIM installer can check for required prerequisites and prompt you to install missing components.

Important prerequisites include:

- Git
- Python
- Windows-compatible development tools

Espressif currently lists **Python 3.10 as the minimum supported version** for ESP-IDF.

Official Windows installation guide:

https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/windows-setup.html

---

## 7.2 Install EIM with WinGet

Open:

**Windows Terminal** or **PowerShell**

For the EIM GUI:

```powershell
winget install Espressif.EIM
```

For the CLI-only package:

```powershell
winget install Espressif.EIM-CLI
```

The GUI version is recommended for a normal desktop development setup.

---

## 7.3 Install ESP-IDF v5.5.1

After installing EIM, you can use its GUI or CLI.

### CLI

Run:

```powershell
eim install -i v5.5.1
```

If you want the interactive installer:

```powershell
eim wizard
```

The wizard allows you to select the ESP-IDF version and installation options.

---

## 7.4 GUI Installation

Launch:

**ESP-IDF Installation Manager**

Choose:

**New Installation → Start Installation**

For a new user, **Easy Installation** is the simplest option.

However, because this project requires a specific ESP-IDF version, use **Custom Installation** when necessary and select:

```text
ESP-IDF v5.5.1
```

EIM's official documentation states that Custom Installation should be used when selecting a specific ESP-IDF version or customizing the installation path.

---

## 7.5 Installation Path

A typical Windows ESP-IDF installation is under:

```text
C:\Espressif
```

Using a simple path is recommended.

Avoid putting ESP-IDF projects in paths containing spaces. Espressif's ESP-IDF build system documentation specifically warns that spaces are not supported in ESP-IDF/project paths.

A good project location is:

```text
C:\Users\<username>\esp\
```

rather than:

```text
C:\Users\<username>\My Projects\ESP32 Dashboard\
```

---

# 8. Activate ESP-IDF on Windows

EIM provides activated ESP-IDF terminals.

You can launch the ESP-IDF terminal through the EIM GUI.

You can also use:

```powershell
eim shell v5.5.1
```

This opens a shell with the selected ESP-IDF environment.

On Windows, EIM may also create shortcuts such as an IDF PowerShell shortcut.

---

# 9. Verify ESP-IDF on Windows

Inside the activated ESP-IDF terminal:

```powershell
idf.py --version
```

For this Dashboard project, you should see:

```text
ESP-IDF v5.5.1
```

Also check:

```powershell
python --version
```

and:

```powershell
git --version
```

---

# 10. Linux — Install ESP-IDF

Espressif's current Linux installation method also uses EIM.

Official guide:

https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/linux-setup.html

EIM supports Debian/Ubuntu, RPM-based distributions and other installation methods.

---

# 11. Ubuntu / Debian

## 11.1 Add the Espressif EIM repository

Run:

```bash
echo "deb [trusted=yes] https://dl.espressif.com/dl/eim/apt/ stable main" | sudo tee /etc/apt/sources.list.d/espressif.list
```

Update package information:

```bash
sudo apt update
```

Install the GUI + CLI:

```bash
sudo apt install eim
```

Or install CLI only:

```bash
sudo apt install eim-cli
```

---

## 11.2 Install ESP-IDF v5.5.1

Run:

```bash
eim install -i v5.5.1
```

Or use the interactive wizard:

```bash
eim wizard
```

---

## 11.3 Activate ESP-IDF

Use:

```bash
eim shell v5.5.1
```

Alternatively, EIM provides an activation script in the installation directory.

The activation script should be sourced into the current shell.

For example:

```bash
source "/path/to/activate_idf_v5.5.1.sh"
```

---

# 12. Fedora / RHEL / RPM-Based Linux

Espressif provides an EIM repository for RPM-based Linux distributions.

Create the repository:

```bash
sudo tee /etc/yum.repos.d/espressif-eim.repo << 'EOF'
[eim]
name=ESP-IDF Installation Manager
baseurl=https://dl.espressif.com/dl/eim/rpm/$basearch
enabled=1
gpgcheck=0
EOF
```

Install EIM:

```bash
sudo dnf install eim
```

Or CLI only:

```bash
sudo dnf install eim-cli
```

Install the project ESP-IDF version:

```bash
eim install -i v5.5.1
```

Activate:

```bash
eim shell v5.5.1
```

Verify:

```bash
idf.py --version
```

---

# 13. Other Linux Distributions

EIM also supports additional installation methods, including:

- direct downloads
- Homebrew
- GUI
- CLI

Consult the current official EIM documentation for your specific distribution:

https://docs.espressif.com/projects/idf-im-ui/en/latest/

The key project command remains:

```bash
eim install -i v5.5.1
```

---

# 14. Linux Serial Port Permissions

When connecting the CrowPanel to Linux, the ESP32-S3 may appear as:

```text
/dev/ttyUSB0
```

or:

```text
/dev/ttyACM0
```

If you receive a permission error, add your user to the `dialout` group:

```bash
sudo usermod -aG dialout $USER
```

Then log out and log in again.

Reconnect the CrowPanel and check:

```bash
ls /dev/ttyUSB*
```

or:

```bash
ls /dev/ttyACM*
```

---

# 15. macOS — Install ESP-IDF

Official Espressif macOS documentation:

https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/macos-setup.html

EIM is the recommended installation method.

---

# 16. Install Homebrew on macOS

If Homebrew is not already installed:

https://brew.sh/

The standard installation command is:

```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

Follow the instructions displayed by Homebrew.

---

# 17. Install EIM on macOS

Add Espressif's Homebrew tap:

```bash
brew tap espressif/eim
```

Install the EIM CLI:

```bash
brew install eim
```

Or install the GUI, which includes the CLI:

```bash
brew install --cask eim-gui
```

---

# 18. Install ESP-IDF v5.5.1 on macOS

Run:

```bash
eim install -i v5.5.1
```

Or:

```bash
eim wizard
```

Select:

```text
ESP-IDF v5.5.1
```

---

# 19. Activate ESP-IDF on macOS

Run:

```bash
eim shell v5.5.1
```

Or source the activation script created by EIM:

```bash
source "/path/to/activate_idf_v5.5.1.sh"
```

Then verify:

```bash
idf.py --version
```

Expected:

```text
ESP-IDF v5.5.1
```

---

# 20. Find the CrowPanel Serial Port on macOS

Connect the CrowPanel using a USB data cable.

List serial devices:

```bash
ls /dev/cu.*
```

You may see something similar to:

```text
/dev/cu.usbmodemXXXX
```

The exact name depends on the connected USB device and macOS version.

---

# 21. Install Visual Studio Code

The official VS Code download page is:

https://code.visualstudio.com/download

VS Code supports:

- Windows
- Linux
- macOS
- Intel Macs
- Apple Silicon Macs

---

# 22. Windows — Install VS Code

Download VS Code:

https://code.visualstudio.com/download

Run the installer.

For most users, the **User Setup** installation is convenient because it does not require administrator privileges.

Official Windows setup:

https://code.visualstudio.com/docs/setup/windows

Launch VS Code after installation.

---

# 23. Linux — Install VS Code

Download the appropriate package from:

https://code.visualstudio.com/download

For Ubuntu/Debian, install the `.deb` package.

Example:

```bash
sudo apt install ./<downloaded-file>.deb
```

For Fedora/RHEL, install the `.rpm` package:

```bash
sudo dnf install ./<downloaded-file>.rpm
```

VS Code is also available through Snap:

```bash
sudo snap install --classic code
```

> For ESP-IDF development, if the Snap installation causes EIM/GUI integration problems, Espressif's extension troubleshooting documentation recommends using the official `.deb` installation instead of the Snap package.

Official Linux installation:

https://code.visualstudio.com/docs/setup/linux

---

# 24. macOS — Install VS Code

Download the macOS version:

https://code.visualstudio.com/download

Then:

1. Open the downloaded `.dmg`.
2. Drag **Visual Studio Code.app** to **Applications**.
3. Launch VS Code.

Official macOS installation:

https://code.visualstudio.com/docs/setup/mac

---

# 25. Install the ESP-IDF Extension in VS Code

Open Visual Studio Code.

Go to:

**View → Extensions**

or use:

```text
Ctrl + Shift + X
```

on Windows/Linux.

On macOS:

```text
Shift + Command + X
```

Search:

```text
ESP-IDF
```

Install the extension published by:

**Espressif Systems**

Official extension documentation:

https://docs.espressif.com/projects/vscode-esp-idf-extension/en/latest/

The extension provides integrated tools for:

- building
- flashing
- monitoring
- debugging
- project creation
- configuration
- serial-port selection
- component management
- partition editing
- size analysis
- troubleshooting

---

# 26. Configure the ESP-IDF Extension

After installing the extension, open the Command Palette:

**View → Command Palette**

or:

```text
Ctrl + Shift + P
```

Search for:

```text
ESP-IDF: Select Current ESP-IDF Version
```

Select the ESP-IDF installation required by the project.

For our existing Dashboard firmware:

```text
ESP-IDF v5.5.1
```

The extension automatically detects ESP-IDF versions installed through EIM by reading EIM's `eim_idf.json`.

The default locations are:

### Windows

```text
C:\Espressif\tools\eim_idf.json
```

### macOS/Linux

```text
$HOME/.espressif/tools/eim_idf.json
```

If EIM uses a different location, the extension provides:

```text
idf.eimIdfJsonPath
```

to specify the location manually.

---

# 27. Verify the VS Code ESP-IDF Environment

After selecting the ESP-IDF version:

Open the Command Palette and run:

```text
ESP-IDF: Doctor Command
```

The Doctor command checks the ESP-IDF development environment and helps identify configuration problems.

Check that the extension can locate:

- ESP-IDF
- Python environment
- toolchain
- CMake
- Ninja
- flashing tools
- selected ESP-IDF version

---

# 28. Open the CrowPanel Dashboard Project

The Dashboard is an ESP-IDF project.

Open VS Code:

**File → Open Folder**

Select the **root folder of the Dashboard firmware**.

The project should contain an ESP-IDF project structure such as:

```text
Dashboard/
├── CMakeLists.txt
├── sdkconfig
├── main/
│   ├── CMakeLists.txt
│   ├── home_control.c
│   ├── home_ui_control.c
│   └── ...
├── components/
└── ...
```

The exact files depend on the version of the project.

---

# 29. Existing Dashboard UI

Our Dashboard UI was developed using **SquareLine Studio** and **LVGL**.

The project contains generated LVGL UI files such as:

```text
ui.c
ui.h
ui_helpers.c
ui_helpers.h
ui_events.h
ui_Screen1.c
ui_Screen1.h
ui_Screen2.c
ui_Screen2.h
ui_comp.c
ui_comp.h
...
```

These generated files should normally be treated carefully when making UI changes.

If the UI is regenerated from SquareLine Studio, changes made directly to generated files can be overwritten.

---

# 30. Dashboard Hardware and I²C

The CrowPanel Advance 7 uses an I²C bus for peripheral communication.

For our Dashboard hardware configuration, the I²C pins used are:

```text
SDA → GPIO15
SCL → GPIO16
```

The board's I²C-connected peripherals include devices such as the touch/controller interface and board control hardware.

> Always use the GPIO definitions from the project source and the specific CrowPanel hardware documentation when modifying the wiring. Different hardware revisions or example projects can use different definitions.

---

# 31. Display and Touch

According to Elecrow's official documentation for the 7-inch Advance HMI:

```text
Display resolution: 800 × 480
Display driver:     SC7277
Touch controller:   GT911
MCU:                ESP32-S3
```

Official Elecrow Lesson 1:

https://elecrow.com/wiki/advance_idf_lesson1.html

The display and touch hardware are initialized by the board-specific ESP-IDF code before the LVGL user interface is started.

---

# 32. LVGL and the Dashboard

LVGL is the graphics framework used by the Dashboard.

The general software flow is:

```text
ESP-IDF
   │
   ├── ESP32-S3 hardware initialization
   │
   ├── Display initialization
   │
   ├── Touch initialization
   │
   ├── Network initialization
   │
   └── LVGL
         │
         └── Dashboard UI
               │
               ├── Date / Day
               ├── Temperature
               ├── Humidity
               ├── Device status
               └── Home controls
```

Elecrow's official Advance 7 ESP-IDF course also uses LVGL for UI development.

---

# 33. Build the Dashboard Project

Open an ESP-IDF terminal with the project's required version activated.

For our existing Dashboard project:

```text
ESP-IDF v5.5.1
```

Navigate to the project directory:

```bash
cd <path-to-dashboard-project>
```

Build:

```bash
idf.py build
```

If the project has been configured for another target, verify that the target is:

```text
esp32s3
```

You can explicitly set it with:

```bash
idf.py set-target esp32s3
```

> **Warning:** `idf.py set-target` can clear/reinitialize existing build/configuration data. Do not run it unnecessarily on an already configured project.

---

# 34. Flash the CrowPanel

Connect the CrowPanel to the computer using a USB-C **data cable**.

Find the serial port.

### Windows

Example:

```text
COM5
```

### Linux

Example:

```text
/dev/ttyACM0
```

or:

```text
/dev/ttyUSB0
```

### macOS

Example:

```text
/dev/cu.usbmodemXXXX
```

Then flash:

### Windows

```powershell
idf.py -p COM5 flash
```

### Linux

```bash
idf.py -p /dev/ttyACM0 flash
```

### macOS

```bash
idf.py -p /dev/cu.usbmodemXXXX flash
```

Replace the example port with the actual CrowPanel port.

---

# 35. Monitor the Dashboard

After flashing, use the ESP-IDF monitor:

```bash
idf.py monitor
```

Or combine flashing and monitoring:

```bash
idf.py -p PORT flash monitor
```

For example:

```bash
idf.py -p COM5 flash monitor
```

To exit the ESP-IDF monitor:

```text
Ctrl + ]
```

---

# 36. Flash and Monitor from VS Code

The ESP-IDF extension provides integrated commands.

Open:

**View → Command Palette**

Useful commands include:

```text
ESP-IDF: Build your Project
```

```text
ESP-IDF: Flash your Project
```

```text
ESP-IDF: Monitor your Device
```

The extension also provides serial-port selection and other project controls.

---

# 37. Configure the ESP32-S3 Target

The ESP-IDF extension provides:

```text
ESP-IDF: Set Espressif Device Target
```

Select:

```text
esp32s3
```

For the CrowPanel Advance 7, the ESP32-S3 is the target MCU.

You can then configure project settings using:

```text
ESP-IDF: SDK Configuration Editor
```

This opens the graphical Kconfig interface.

---

# 38. Important — Do Not Randomly Change sdkconfig

The project may contain:

```text
sdkconfig
```

This file contains project configuration settings.

Display projects can depend on very specific:

- flash settings
- PSRAM settings
- CPU configuration
- partition table
- USB configuration
- LVGL-related options
- Wi-Fi configuration
- compiler options

Therefore:

> If the Dashboard firmware already builds correctly, do not regenerate or replace `sdkconfig` simply because a newer Elecrow example uses different settings.

Keep the project's configuration intact unless you understand the change you are making.

---

# 39. Troubleshooting

## Problem 1 — `idf.py` is not recognized

Example:

```text
'idf.py' is not recognized...
```

ESP-IDF has probably not been activated.

Use:

```bash
eim shell v5.5.1
```

Then:

```bash
idf.py --version
```

---

## Problem 2 — Wrong ESP-IDF version

Check:

```bash
idf.py --version
```

For our existing Dashboard project:

```text
ESP-IDF v5.5.1
```

If another version is active, use:

```bash
eim shell v5.5.1
```

or:

```text
ESP-IDF: Select Current ESP-IDF Version
```

in VS Code.

---

## Problem 3 — VS Code cannot find ESP-IDF

Run:

```text
ESP-IDF: Doctor Command
```

Then verify that the correct EIM installation is detected.

If EIM's `eim_idf.json` is stored outside the default location, configure:

```text
idf.eimIdfJsonPath
```

in VS Code settings.

---

## Problem 4 — CrowPanel is not detected

Check:

1. USB-C cable supports data.
2. Try another USB port.
3. Reconnect the CrowPanel.
4. Check Windows Device Manager.
5. Check `/dev/ttyACM*` or `/dev/ttyUSB*` on Linux.
6. Check `/dev/cu.*` on macOS.
7. Try the board's boot/reset procedure if required.

---

## Problem 5 — Linux permission denied

If you receive:

```text
Permission denied
```

while opening the serial device:

```bash
sudo usermod -aG dialout $USER
```

Log out and log back in.

---

## Problem 6 — Build fails after changing ESP-IDF version

First return to the version used by the project:

```text
ESP-IDF v5.5.1
```

Then perform a clean build:

```bash
idf.py fullclean
idf.py build
```

Do not immediately modify source code to make it work with a different ESP-IDF version.

---

## Problem 7 — Display remains blank

Check:

- ESP-IDF version
- correct target: `esp32s3`
- flash configuration
- PSRAM configuration
- display initialization
- board hardware revision
- display driver configuration
- LVGL initialization
- serial monitor logs

Also compare your project with the correct Elecrow hardware-version documentation.

---

## Problem 8 — Touchscreen does not respond

Check:

- GT911 initialization
- I²C wiring/configuration
- I²C SDA/SCL definitions
- touch interrupt/reset configuration
- board revision
- LVGL input-device initialization

For our Dashboard hardware configuration, the relevant I²C pins are:

```text
SDA → GPIO15
SCL → GPIO16
```

---

## Problem 9 — LVGL UI is not displayed correctly

Check:

- LVGL version
- display driver configuration
- screen resolution
- color format
- framebuffer configuration
- PSRAM configuration
- UI files generated by SquareLine Studio
- ESP-IDF version

The 7-inch display resolution is:

```text
800 × 480
```

---

# 40. Recommended Verification Checklist

## Computer

- [ ] Git installed
- [ ] Python available
- [ ] Visual Studio Code installed

## ESP-IDF

- [ ] ESP-IDF Installation Manager installed
- [ ] ESP-IDF v5.5.1 installed
- [ ] ESP-IDF environment activated
- [ ] `idf.py --version` works
- [ ] ESP32-S3 toolchain installed

## VS Code

- [ ] ESP-IDF extension installed
- [ ] ESP-IDF v5.5.1 selected
- [ ] `ESP-IDF: Doctor Command` works

## CrowPanel

- [ ] USB data cable connected
- [ ] Serial port detected
- [ ] ESP32-S3 selected as target
- [ ] Dashboard project opens successfully
- [ ] Project builds successfully
- [ ] Firmware flashes successfully
- [ ] Serial monitor works
- [ ] Display initializes
- [ ] Touch input works
- [ ] LVGL Dashboard appears

---

# 41. Quick Start

For an experienced developer, the essential setup is:

## Windows

```powershell
winget install Espressif.EIM
eim install -i v5.5.1
eim shell v5.5.1
idf.py --version
```

Expected:

```text
ESP-IDF v5.5.1
```

---

## Ubuntu / Debian

```bash
echo "deb [trusted=yes] https://dl.espressif.com/dl/eim/apt/ stable main" | sudo tee /etc/apt/sources.list.d/espressif.list
sudo apt update
sudo apt install eim
eim install -i v5.5.1
eim shell v5.5.1
idf.py --version
```

---

## macOS

```bash
brew tap espressif/eim
brew install eim
eim install -i v5.5.1
eim shell v5.5.1
idf.py --version
```

Then:

1. Install Visual Studio Code.
2. Install **ESP-IDF** extension by Espressif Systems.
3. Run:
   ```text
   ESP-IDF: Select Current ESP-IDF Version
   ```
4. Select:
   ```text
   ESP-IDF v5.5.1
   ```
5. Run:
   ```text
   ESP-IDF: Doctor Command
   ```
6. Open the Dashboard project.
7. Select `esp32s3`.
8. Build.
9. Flash the CrowPanel.
10. Start the monitor.

---

# 42. Official References

## Espressif ESP-IDF

**ESP-IDF Get Started**  
https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/

**ESP-IDF Installation Manager**  
https://docs.espressif.com/projects/idf-im-ui/en/latest/

**Installation Methods**  
https://docs.espressif.com/projects/idf-im-ui/en/latest/installation_methods.html

**EIM CLI Installation**  
https://docs.espressif.com/projects/idf-im-ui/en/latest/cli_installation.html

**Windows Installation**  
https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/windows-setup.html

**Linux Installation**  
https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/linux-setup.html

**macOS Installation**  
https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/macos-setup.html

**ESP32-S3 Start Project — Windows**  
https://docs.espressif.com/projects/esp-idf/en/stable/esp32s3/get-started/windows-start-project.html

**ESP32-S3 Start Project — Linux/macOS**  
https://docs.espressif.com/projects/esp-idf/en/stable/esp32s3/get-started/linux-macos-start-project.html

---

## ESP-IDF VS Code Extension

**Extension Documentation**  
https://docs.espressif.com/projects/vscode-esp-idf-extension/en/latest/

**Installation**  
https://docs.espressif.com/projects/vscode-esp-idf-extension/en/latest/installation.html

**Create/Open ESP-IDF Project**  
https://docs.espressif.com/projects/vscode-esp-idf-extension/en/latest/startproject.html

**Configure Project**  
https://docs.espressif.com/projects/vscode-esp-idf-extension/en/latest/configureproject.html

**Troubleshooting**  
https://docs.espressif.com/projects/vscode-esp-idf-extension/en/latest/troubleshooting.html

---

## Visual Studio Code

**Download**  
https://code.visualstudio.com/download

**Windows Setup**  
https://code.visualstudio.com/docs/setup/windows

**Linux Setup**  
https://code.visualstudio.com/docs/setup/linux

**macOS Setup**  
https://code.visualstudio.com/docs/setup/mac

---

## Elecrow CrowPanel Advance 7

**CrowPanel Advance HMI ESP-IDF Course**  
https://elecrow.com/wiki/HMI_Display_course_esp_idf.html

**CrowPanel Advance 7.0-inch ESP-IDF Course V1.5**  
https://www.elecrow.com/wiki/CrowPanel_Advance_7.0inch_ESP_IDF_Course_V15.html

**CrowPanel Advance 7 GitHub Repository**  
https://github.com/Elecrow-RD/CrowPanel-Advance-7-HMI-ESP32-S3-AI-Powered-IPS-Touch-Screen-800x480

**Elecrow ESP-IDF Installation Guide**  
https://www.elecrow.com/wiki/ESP_IDF_Installation_Guide.html

**Elecrow Lesson 1 — IDF-IDE Installation and Hello World**  
https://elecrow.com/wiki/advance_idf_lesson1.html

---

# 43. Final Notes

This README is specifically intended for the **CrowPanel Advance 7 Dashboard** in our Voice-Controlled Embedded Dashboard project.

The most important distinction is between:

### Our existing project environment

```text
ESP32-S3
ESP-IDF v5.5.1
LVGL
800 × 480 Dashboard
SquareLine Studio-generated UI
```

and:

### Elecrow's current V1.5 learning course

```text
ESP32-S3
ESP-IDF v5.5.4
LVGL 9.1.0
800 × 480
```

Both are valid development environments, but they should not be mixed casually when reproducing an existing firmware project.

For the repository version of our Dashboard firmware, **follow the project's specified ESP-IDF version first**.

Once the environment is installed and verified, the next step is to open the Dashboard source, select the ESP32-S3 target, build the project, flash the CrowPanel, and verify the LVGL interface and touchscreen through the serial monitor.

---

## Project

**Project:** Voice-Controlled Embedded Dashboard

**Dashboard:** Elecrow CrowPanel Advance 7

**MCU:** ESP32-S3

**Display:** 7-inch IPS, 800 × 480

**UI Framework:** LVGL

**UI Design Tool:** SquareLine Studio

**Development Framework:** ESP-IDF

**Project ESP-IDF Version:** v5.5.1
