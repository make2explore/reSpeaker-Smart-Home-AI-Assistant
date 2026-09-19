# ESP-IDF Development Environment — reSpeaker / XIAO ESP32-S3

This guide explains how to install **ESP-IDF (Espressif IoT Development Framework)** and configure **Visual Studio Code with the official ESP-IDF extension** for the **reSpeaker XMOS XVF3800 + XIAO ESP32-S3** used in this project.

> **Project-specific version:** The reSpeaker/XIAO ESP32-S3 firmware used in this project was developed and built with **ESP-IDF 5.4.2**. For the best chance of reproducing this project exactly, install **ESP-IDF v5.4.2** rather than simply selecting the newest ESP-IDF release.

The official Espressif documentation now recommends the **ESP-IDF Installation Manager (EIM)** for installing ESP-IDF and its required tools. EIM can manage multiple ESP-IDF versions and supports Windows, macOS, and Linux.

---

# 1. What Is ESP-IDF?

**ESP-IDF** is Espressif's official development framework for ESP32-family microcontrollers.

Unlike Arduino sketches, ESP-IDF projects use Espressif's native development environment and provide direct access to:

- FreeRTOS
- Wi-Fi
- Bluetooth / BLE
- I2S audio
- I2C
- GPIO
- USB
- Networking
- Audio and DSP peripherals
- ESP-NETIF
- NVS
- Partition tables
- OTA
- Debugging and tracing
- ESP-specific build and configuration tools

For this project, ESP-IDF is used for the firmware running on the **XIAO ESP32-S3 integrated with the reSpeaker XMOS XVF3800**.

The XIAO handles the embedded AI/voice-assistant application, while the XVF3800 provides the microphone-array and advanced audio-processing hardware.

---

# 2. Important: ESP-IDF Version for This Project

## Use ESP-IDF v5.4.2

The reSpeaker/XIAO ESP32-S3 firmware for this project was developed using:

```text
ESP-IDF v5.4.2
```

You may see newer ESP-IDF versions on Espressif's website. Newer versions are excellent for new projects, but **do not automatically assume that a newer version is compatible with this project's existing firmware**.

Different ESP-IDF releases can change:

- APIs
- component versions
- build-system behavior
- Python environments
- toolchain versions
- Kconfig options
- USB behavior
- audio-related components
- compiler behavior

Therefore, if your goal is to **build and reproduce this project's reSpeaker firmware**, install:

```text
ESP-IDF v5.4.2
```

Espressif's current EIM documentation explicitly supports installing specific ESP-IDF versions, including v5.4.2:

```bash
eim install -i v5.4.2
```

---

# 3. Recommended Installation Architecture

For this project, the recommended development setup is:

```text
                  Your Computer
                       │
          ┌────────────┴────────────┐
          │                         │
     Visual Studio Code         ESP-IDF
          │                         │
          │                ESP-IDF v5.4.2
          │                         │
          └──────────┬──────────────┘
                     │
             ESP-IDF Extension
                     │
                     ▼
            XIAO ESP32-S3
                     │
                  I2S / I2C
                     │
                     ▼
          reSpeaker XMOS XVF3800
                     │
          ┌──────────┴──────────┐
          │                     │
      4-Mic Array          Audio Output
```

The **ESP-IDF Installation Manager (EIM)** installs ESP-IDF and the required development tools.

The **ESP-IDF VS Code extension** provides the development interface for:

- configuring projects
- compiling
- flashing
- monitoring
- debugging
- selecting ESP-IDF versions
- managing serial ports
- diagnosing installation problems

---

# 4. Before You Start

You will need:

### Hardware

- Seeed Studio **reSpeaker XMOS XVF3800 with XIAO ESP32-S3**
- USB-C **data** cable
- Computer running:
  - Windows
  - Linux
  - macOS

### Software

- Git
- Python
- ESP-IDF v5.4.2
- ESP-IDF tools/toolchain
- Visual Studio Code
- ESP-IDF Extension for VS Code

> The current EIM checks the required prerequisites and can install missing prerequisites on supported platforms. Espressif currently lists Python 3.10–3.14 as supported by EIM.

---

# 5. Official Espressif Resources

Before following this guide, it is useful to bookmark these official resources.

### ESP-IDF Get Started

https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/

### ESP-IDF Installation Manager

https://docs.espressif.com/projects/idf-im-ui/en/latest/

### EIM Installation Methods

https://docs.espressif.com/projects/idf-im-ui/en/latest/installation_methods.html

### EIM CLI Installation

https://docs.espressif.com/projects/idf-im-ui/en/latest/cli_installation.html

### EIM Prerequisites

https://docs.espressif.com/projects/idf-im-ui/en/latest/prerequisites.html

### ESP-IDF VS Code Extension

https://docs.espressif.com/projects/vscode-esp-idf-extension/en/latest/

### ESP-IDF Extension Installation

https://docs.espressif.com/projects/vscode-esp-idf-extension/en/latest/installation.html

### ESP-IDF v5.4.2 Documentation

https://docs.espressif.com/projects/esp-idf/en/v5.4.2/

### Seeed Studio — reSpeaker XVF3800 + XIAO ESP32-S3

https://wiki.seeedstudio.com/respeaker_xvf3800_xiao_getting_started/

### Seeed Studio — Xiaozhi + reSpeaker

https://wiki.seeedstudio.com/respeaker_xvf_3800_xiaozhi/

---

# 6. Install ESP-IDF on Windows

## 6.1 Recommended Method — ESP-IDF Installation Manager

Espressif's current recommended Windows installation method is the **ESP-IDF Installation Manager (EIM)**.

EIM provides:

- GUI installation
- CLI installation
- multiple ESP-IDF version management
- toolchain installation
- Python environment management
- offline installation support

Official Windows documentation:

https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/windows-setup.html

---

## 6.2 Install EIM Using WinGet

Open **PowerShell** or **Windows Terminal**.

Install the EIM GUI:

```powershell
winget install Espressif.EIM
```

Or install the CLI-only version:

```powershell
winget install Espressif.EIM-CLI
```

For this project, the GUI version is convenient because it makes version management easier.

---

## 6.3 Install ESP-IDF v5.4.2

After EIM is installed, you can use the GUI or CLI.

### CLI method

Open PowerShell and run:

```powershell
eim install -i v5.4.2
```

This installs ESP-IDF v5.4.2 and the corresponding tools.

To see the available EIM commands:

```powershell
eim --help
```

To start the interactive installation wizard:

```powershell
eim wizard
```

---

## 6.4 GUI Method

Launch **ESP-IDF Installation Manager**.

Select:

**New Installation → Start Installation**

For a standard installation, choose:

**Easy Installation**

For this project, however, make sure that the selected ESP-IDF version is:

```text
v5.4.2
```

If you need to select a specific version or customize the installation path, use **Expert / Custom Installation**.

---

## 6.5 Installation Location

A typical Windows installation may use:

```text
C:\Espressif
```

You can choose another location during installation.

Avoid unusual paths with unnecessary complexity.

For older ESP-IDF releases, spaces in configured paths can cause problems. Keeping the ESP-IDF environment in a simple path such as:

```text
C:\Espressif
```

is a good practice.

---

## 6.6 Activate ESP-IDF on Windows

After installation, EIM creates an environment that provides access to:

- `idf.py`
- compiler/toolchain
- CMake
- Ninja
- esptool
- other ESP-IDF utilities

EIM can provide an **IDF PowerShell** shortcut.

You can also use the EIM CLI shell:

```powershell
eim shell v5.4.2
```

This opens a shell with ESP-IDF v5.4.2 activated.

---

## 6.7 Verify the Installation

Inside the ESP-IDF environment, run:

```powershell
idf.py --version
```

You should see a result corresponding to:

```text
ESP-IDF v5.4.2
```

You can also verify the Python environment:

```powershell
python --version
```

And check Git:

```powershell
git --version
```

---

# 7. Install ESP-IDF on Linux

Espressif's current Linux installation method also uses EIM.

Official documentation:

https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/linux-setup.html

EIM supports:

- Debian/Ubuntu
- Fedora/RHEL and other RPM-based distributions
- Arch-based distributions
- Homebrew
- direct downloads

---

# 8. Ubuntu / Debian Linux

## 8.1 Install EIM

The current Espressif documentation provides an official APT repository.

Create the keyring directory:

```bash
sudo install -m 0755 -d /etc/apt/keyrings
```

Download the Espressif signing key:

```bash
sudo curl -fsSL https://dl.espressif.com/dl/eim/eim.gpg \
  -o /etc/apt/keyrings/eim.gpg
```

Set the appropriate permissions:

```bash
sudo chmod 0644 /etc/apt/keyrings/eim.gpg
```

Add the official Espressif repository:

```bash
sudo curl -fsSL https://dl.espressif.com/dl/eim/eim.sources \
  -o /etc/apt/sources.list.d/espressif.sources
```

Update the package list:

```bash
sudo apt update
```

Install EIM:

```bash
sudo apt install eim
```

If you only want the command-line version:

```bash
sudo apt install eim-cli
```

---

## 8.2 Install ESP-IDF v5.4.2

Run:

```bash
eim install -i v5.4.2
```

Or use:

```bash
eim wizard
```

for an interactive installation.

---

## 8.3 Activate ESP-IDF

Use:

```bash
eim shell v5.4.2
```

Alternatively, EIM creates activation scripts in the installation environment.

The activation script must be **sourced**, not executed as a standalone process.

For example:

```bash
source /path/to/activate_idf_v5.4.2.sh
```

---

## 8.4 Verify

Run:

```bash
idf.py --version
```

Expected:

```text
ESP-IDF v5.4.2
```

Also verify:

```bash
python3 --version
git --version
```

---

# 9. Linux Serial-Port Permissions

When flashing the XIAO ESP32-S3 from Linux, you may receive a permission error when accessing the USB serial device.

The device may appear as:

```text
/dev/ttyUSB0
```

or:

```text
/dev/ttyACM0
```

Add your user to the `dialout` group:

```bash
sudo usermod -aG dialout $USER
```

Then **log out and log back in**.

After reconnecting the board, check the device:

```bash
ls /dev/ttyUSB*
```

or:

```bash
ls /dev/ttyACM*
```

---

# 10. Fedora / RHEL / RPM-Based Linux

Espressif provides an official RPM repository for EIM.

Import the Espressif signing key:

```bash
sudo rpm --import https://dl.espressif.com/dl/eim/eim.asc
```

Add the EIM repository:

```bash
sudo curl -fsSL https://dl.espressif.com/dl/eim/rpm/eim.repo \
  -o /etc/yum.repos.d/espressif-eim.repo
```

Install EIM:

```bash
sudo dnf install eim
```

Or CLI-only:

```bash
sudo dnf install eim-cli
```

Then install the project version:

```bash
eim install -i v5.4.2
```

Activate it:

```bash
eim shell v5.4.2
```

Verify:

```bash
idf.py --version
```

---

# 11. Arch Linux

EIM also provides an official package repository for Arch-based distributions.

The setup involves importing Espressif's signing key, adding the EIM repository to `/etc/pacman.conf`, synchronizing package databases, and installing EIM.

Refer to the official EIM documentation for the current Arch repository instructions:

https://docs.espressif.com/projects/idf-im-ui/en/latest/

Then install ESP-IDF:

```bash
eim install -i v5.4.2
```

Activate:

```bash
eim shell v5.4.2
```

Verify:

```bash
idf.py --version
```

---

# 12. Install ESP-IDF on macOS

Espressif recommends EIM for macOS as well.

Official documentation:

https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/macos-setup.html

---

## 12.1 Install Homebrew

If Homebrew is not already installed, install it from:

https://brew.sh/

The standard Homebrew installation command is:

```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

Follow the instructions displayed by Homebrew after installation.

---

## 12.2 Install EIM

Add the Espressif Homebrew tap:

```bash
brew tap espressif/eim
```

Install the GUI:

```bash
brew install --cask eim-gui
```

Or install the CLI:

```bash
brew install eim
```

---

## 12.3 Install ESP-IDF v5.4.2

Run:

```bash
eim install -i v5.4.2
```

Or:

```bash
eim wizard
```

and select ESP-IDF v5.4.2.

---

## 12.4 macOS Prerequisites

Depending on the ESP-IDF version and tools you use, additional packages may be required.

Espressif's current macOS documentation lists packages such as:

```bash
brew install libgcrypt glib pixman sdl2 libslirp dfu-util cmake python
```

Use the official version-specific documentation if EIM reports a missing prerequisite.

---

## 12.5 Activate ESP-IDF

Use:

```bash
eim shell v5.4.2
```

Or source the generated activation script:

```bash
source /path/to/activate_idf_v5.4.2.sh
```

For Bash/Zsh, the activation script must be **sourced**.

---

## 12.6 Verify

Run:

```bash
idf.py --version
```

Expected:

```text
ESP-IDF v5.4.2
```

Also check:

```bash
python3 --version
git --version
```

---

# 13. Install Visual Studio Code

The ESP-IDF extension integrates ESP-IDF directly into VS Code.

Official VS Code download:

https://code.visualstudio.com/download

Official setup documentation:

https://code.visualstudio.com/docs/getstarted/overview

---

# 14. Install VS Code on Windows

Download the **User Setup** version of VS Code.

Run the installer and follow the setup wizard.

The User Setup is recommended by Microsoft for most users because it does not require administrator privileges and provides a smooth update experience.

Official documentation:

https://code.visualstudio.com/docs/setup/windows

After installation, open VS Code.

---

# 15. Install VS Code on Linux

For Ubuntu/Debian:

1. Download the `.deb` package.
2. Install it using the software center, or:

```bash
sudo apt install ./<downloaded-file>.deb
```

For Fedora/RHEL:

Download the `.rpm` package and install it with:

```bash
sudo dnf install ./<downloaded-file>.rpm
```

VS Code is also officially available through Snap:

```bash
sudo snap install --classic code
```

Official Linux installation guide:

https://code.visualstudio.com/docs/setup/linux

---

# 16. Install VS Code on macOS

Download the macOS version from:

https://code.visualstudio.com/download

Then:

1. Open the downloaded `.dmg`.
2. Drag **Visual Studio Code.app** into **Applications**.
3. Launch VS Code.

VS Code supports both Intel and Apple Silicon Macs.

Official macOS installation guide:

https://code.visualstudio.com/docs/setup/mac

---

# 17. Install the ESP-IDF VS Code Extension

Open VS Code.

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

Search for:

```text
ESP-IDF
```

Install the extension published by **Espressif Systems**.

Official extension documentation:

https://docs.espressif.com/projects/vscode-esp-idf-extension/en/latest/

---

# 18. Configure the ESP-IDF Extension

After installing the extension:

Open the Command Palette:

**View → Command Palette**

or:

```text
Ctrl + Shift + P
```

Search for:

```text
ESP-IDF: Open ESP-IDF Installation Manager
```

The extension can launch EIM and install ESP-IDF and its tools.

If ESP-IDF was already installed using EIM, the extension can automatically detect installed ESP-IDF versions.

---

# 19. Select ESP-IDF v5.4.2 in VS Code

This step is particularly important for this project.

Open:

**View → Command Palette**

Search for:

```text
ESP-IDF: Select Current ESP-IDF Version
```

Select it.

You should see the available ESP-IDF installations.

Choose:

```text
ESP-IDF v5.4.2
```

The extension will configure the environment for the selected project/workspace.

---

# 20. Verify the ESP-IDF Extension

Open the Command Palette and run:

```text
ESP-IDF: Doctor Command
```

The Doctor command checks the ESP-IDF development environment and helps identify configuration problems.

If the installation is correct, the extension should be able to locate:

- ESP-IDF
- Python environment
- ESP-IDF tools
- compiler/toolchain
- CMake
- Ninja
- serial tools

---

# 21. Open the reSpeaker Project

Once ESP-IDF and the VS Code extension are configured:

1. Open VS Code.
2. Select:
   **File → Open Folder**
3. Open the root folder of the reSpeaker ESP-IDF project.

An ESP-IDF project normally contains files/folders such as:

```text
project/
├── CMakeLists.txt
├── sdkconfig
├── sdkconfig.defaults
├── main/
│   ├── CMakeLists.txt
│   └── ...
├── components/
└── ...
```

The exact structure depends on the project.

---

# 22. Build the reSpeaker Firmware

Open the project in VS Code with ESP-IDF v5.4.2 selected.

You can build from the VS Code ESP-IDF interface or from an activated terminal.

From the project directory:

```bash
idf.py build
```

A successful build should end with a message indicating that the project build completed successfully.

---

# 23. Flash the XIAO ESP32-S3

Connect the XIAO ESP32-S3 to your computer using a USB-C **data cable**.

Find the serial port.

### Windows

Usually:

```text
COM3
COM4
COM5
```

You can check it in:

**Device Manager → Ports (COM & LPT)**

### Linux

Usually:

```text
/dev/ttyUSB0
```

or:

```text
/dev/ttyACM0
```

### macOS

Usually something similar to:

```text
/dev/cu.usbmodemXXXX
```

You can list serial devices with:

```bash
ls /dev/cu.*
```

---

# 24. Flash and Monitor from the Terminal

For example:

### Windows

```powershell
idf.py -p COM3 flash monitor
```

### Linux

```bash
idf.py -p /dev/ttyUSB0 flash monitor
```

### macOS

```bash
idf.py -p /dev/cu.usbmodemXXXX flash monitor
```

Replace the port with the actual port of your XIAO.

You can also let ESP-IDF attempt to detect the port automatically:

```bash
idf.py flash monitor
```

---

# 25. Flash and Monitor from VS Code

The ESP-IDF extension provides buttons and commands for common operations.

Open the Command Palette and search for commands such as:

```text
ESP-IDF: Build your Project
```

```text
ESP-IDF: Flash your Project
```

```text
ESP-IDF: Monitor your Device
```

The extension also provides serial-port selection and other project management tools.

---

# 26. reSpeaker-Specific Considerations

This project uses:

```text
XIAO ESP32-S3
        │
        │ I2S + I2C
        ▼
XMOS XVF3800
        │
        ├── 4-Microphone Array
        ├── AEC
        ├── Beamforming
        ├── DoA
        ├── AGC
        ├── Noise Suppression
        └── Audio Processing
```

Seeed Studio documents that the reSpeaker XVF3800 with XIAO ESP32-S3 supports:

- Stereo I2S input/output
- I2C configuration/control of the XVF3800
- 4-microphone far-field capture
- AEC
- AGC
- DoA
- beamforming
- dereverberation
- noise suppression
- 360° voice capture

Official Seeed Studio guide:

https://wiki.seeedstudio.com/respeaker_xvf3800_xiao_getting_started/

---

# 27. Important: reSpeaker Firmware Mode

The reSpeaker XVF3800 supports different operating modes.

For the XIAO ESP32-S3 integration, Seeed's documentation specifies that the reSpeaker needs the appropriate **I2S firmware**.

In I2S mode, the reSpeaker is not presented to the computer as a normal USB audio device.

Seeed documents the firmware-mode switching and recovery procedure in the official reSpeaker guide.

Before troubleshooting ESP-IDF or the XIAO firmware, make sure the XVF3800 firmware mode is appropriate for the XIAO integration.

Official guide:

https://wiki.seeedstudio.com/respeaker_xvf3800_xiao_getting_started/

---

# 28. Xiaozhi + reSpeaker

This project uses the **Xiaozhi AI voice-assistant firmware/application** with the reSpeaker XVF3800 and XIAO ESP32-S3.

Seeed Studio provides a dedicated guide:

https://wiki.seeedstudio.com/respeaker_xvf_3800_xiaozhi/

That guide describes the ESP32S3 + reSpeaker architecture and provides additional information about firmware preparation and ESP32-side development.

For this project's existing firmware, use:

```text
ESP-IDF v5.4.2
```

---

# 29. Multiple ESP-IDF Versions

It is common for embedded developers to work with multiple ESP-IDF versions.

For example:

```text
ESP-IDF v5.4.2  → reSpeaker / XIAO project
ESP-IDF v5.x    → other ESP32 projects
ESP-IDF v6.x    → newer projects
```

EIM is designed to manage multiple versions.

List installed versions:

```bash
eim list
```

Open a shell using a specific version:

```bash
eim shell v5.4.2
```

This is preferable to changing global environment variables manually.

In VS Code, use:

```text
ESP-IDF: Select Current ESP-IDF Version
```

to select the version for the current project.

---

# 30. Common Problems and Solutions

## Problem 1 — `idf.py` is not recognized

Example:

```text
'idf.py' is not recognized...
```

### Solution

ESP-IDF has probably not been activated.

Use:

```bash
eim shell v5.4.2
```

Then check:

```bash
idf.py --version
```

---

## Problem 2 — Wrong ESP-IDF version

Check:

```bash
idf.py --version
```

For this project it should report:

```text
ESP-IDF v5.4.2
```

If another version is active, use:

```bash
eim shell v5.4.2
```

or select v5.4.2 through:

```text
ESP-IDF: Select Current ESP-IDF Version
```

---

## Problem 3 — VS Code cannot find ESP-IDF

Run:

```text
ESP-IDF: Doctor Command
```

Then verify the selected ESP-IDF version.

The ESP-IDF extension normally detects EIM installations automatically.

If the EIM configuration file is stored in a non-default location, the extension documentation provides the `idf.eimIdfJsonPath` setting for specifying its location.

---

## Problem 4 — XIAO does not appear as a serial port

Try:

1. Use a different USB-C cable.
2. Make sure it is a **data cable**.
3. Disconnect/reconnect the board.
4. Check Device Manager on Windows.
5. Check `/dev/ttyUSB*` or `/dev/ttyACM*` on Linux.
6. Check `/dev/cu.*` on macOS.
7. Try the XIAO bootloader/reset procedure if required.

---

## Problem 5 — Linux permission denied

If Linux reports permission errors when accessing the serial port:

```bash
sudo usermod -aG dialout $USER
```

Log out and log back in.

---

## Problem 6 — Build fails after changing ESP-IDF version

This can happen when a project depends on a particular ESP-IDF release or component version.

For this project, first return to:

```text
ESP-IDF v5.4.2
```

Then perform a clean build:

```bash
idf.py fullclean
idf.py build
```

Do not immediately modify source code to accommodate a different ESP-IDF version unless you intend to port the project.

---

## Problem 7 — reSpeaker is not working even though ESP-IDF builds

Remember that the reSpeaker XVF3800 and XIAO ESP32-S3 are two separate pieces of the system.

Check:

- XVF3800 firmware mode
- I2S connection
- I2C connection
- XIAO power
- USB cable
- speaker/output connection
- microphone orientation
- firmware compatibility

Seeed's official reSpeaker documentation should be checked before changing the ESP-IDF project.

---

# 31. Recommended Verification Checklist

After installation, verify each layer independently.

### Computer

- [ ] Git installed
- [ ] Python available
- [ ] VS Code installed

### ESP-IDF

- [ ] EIM installed
- [ ] ESP-IDF v5.4.2 installed
- [ ] ESP-IDF environment activated
- [ ] `idf.py --version` works
- [ ] Correct toolchain installed

### VS Code

- [ ] ESP-IDF extension installed
- [ ] ESP-IDF v5.4.2 selected
- [ ] `ESP-IDF: Doctor Command` runs successfully

### Hardware

- [ ] XIAO ESP32-S3 connected
- [ ] Serial port detected
- [ ] reSpeaker XVF3800 connected correctly
- [ ] XVF3800 is using the appropriate I2S firmware

### Project

- [ ] Project opens in VS Code
- [ ] `idf.py build` succeeds
- [ ] Firmware flashes successfully
- [ ] Serial monitor displays firmware output

---

# 32. Quick Start — Experienced Users

If you already have VS Code and Git installed, the essential setup is:

## Windows

```powershell
winget install Espressif.EIM
eim install -i v5.4.2
eim shell v5.4.2
idf.py --version
```

## Ubuntu / Debian

```bash
sudo install -m 0755 -d /etc/apt/keyrings
sudo curl -fsSL https://dl.espressif.com/dl/eim/eim.gpg \
  -o /etc/apt/keyrings/eim.gpg
sudo chmod 0644 /etc/apt/keyrings/eim.gpg
sudo curl -fsSL https://dl.espressif.com/dl/eim/eim.sources \
  -o /etc/apt/sources.list.d/espressif.sources
sudo apt update
sudo apt install eim
eim install -i v5.4.2
eim shell v5.4.2
idf.py --version
```

## macOS

```bash
brew tap espressif/eim
brew install eim
eim install -i v5.4.2
eim shell v5.4.2
idf.py --version
```

Then install the **ESP-IDF** extension in VS Code and select:

```text
ESP-IDF: Select Current ESP-IDF Version
        ↓
ESP-IDF v5.4.2
```

---

# 33. Official References

## Espressif

**ESP-IDF Get Started**  
https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/

**ESP-IDF Installation Manager**  
https://docs.espressif.com/projects/idf-im-ui/en/latest/

**EIM Installation Methods**  
https://docs.espressif.com/projects/idf-im-ui/en/latest/installation_methods.html

**EIM Prerequisites**  
https://docs.espressif.com/projects/idf-im-ui/en/latest/prerequisites.html

**EIM CLI Installation**  
https://docs.espressif.com/projects/idf-im-ui/en/latest/cli_installation.html

**EIM After Installing**  
https://docs.espressif.com/projects/idf-im-ui/en/latest/after_installing.html

**ESP-IDF Tools**  
https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/tools/idf-tools.html

**ESP-IDF v5.4.2 Documentation**  
https://docs.espressif.com/projects/esp-idf/en/v5.4.2/

**ESP-IDF v5.4.2 Windows Documentation**  
https://docs.espressif.com/projects/esp-idf/en/v5.4.2/esp32c61/get-started/windows-setup.html

## Visual Studio Code

**VS Code Download**  
https://code.visualstudio.com/download

**VS Code Getting Started**  
https://code.visualstudio.com/docs/getstarted/overview

**VS Code Windows Installation**  
https://code.visualstudio.com/docs/setup/windows

**VS Code Linux Installation**  
https://code.visualstudio.com/docs/setup/linux

**VS Code macOS Installation**  
https://code.visualstudio.com/docs/setup/mac

## Espressif VS Code Extension

**ESP-IDF Extension Documentation**  
https://docs.espressif.com/projects/vscode-esp-idf-extension/en/latest/

**Extension Installation**  
https://docs.espressif.com/projects/vscode-esp-idf-extension/en/latest/installation.html

**Create an ESP-IDF Project**  
https://docs.espressif.com/projects/vscode-esp-idf-extension/en/latest/startproject.html

## Seeed Studio

**reSpeaker XVF3800 + XIAO ESP32-S3 Getting Started**  
https://wiki.seeedstudio.com/respeaker_xvf3800_xiao_getting_started/

**Xiaozhi AI + reSpeaker XVF3800 + XIAO ESP32-S3**  
https://wiki.seeedstudio.com/respeaker_xvf_3800_xiaozhi/

---

# 34. Final Notes

The goal of this README is to make the **reSpeaker portion of the project reproducible**, while keeping the installation procedure aligned with Espressif's current tooling.

The most important point for this particular project is:

> **Use ESP-IDF v5.4.2 when building the existing reSpeaker/XIAO ESP32-S3 firmware.**

For new ESP32 projects, you should consult Espressif's current documentation and select the ESP-IDF version appropriate for that project.

Once ESP-IDF and the VS Code extension are correctly configured, the next step is to open the reSpeaker project, select **ESP-IDF v5.4.2**, build the firmware, flash the XIAO ESP32-S3, and monitor the device output.

**Project:** reSpeaker Smart Home AI Assistant  
**Voice Assistant:** Joy  
**Voice Hardware:** Seeed Studio reSpeaker XMOS XVF3800 + XIAO ESP32-S3  
**Framework:** ESP-IDF v5.4.2
