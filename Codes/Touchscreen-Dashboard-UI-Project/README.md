# SquareLine Studio Project

This folder contains the editable **SquareLine Studio UI project** for
the touchscreen dashboard used in the **reSpeaker Smart Home AI
Assistant** project.

The dashboard runs on the **7-inch CrowPanel Advance touchscreen
(ESP32-S3)** and provides a visual interface for the smart-home system.
SquareLine Studio is used to design the graphical interface, with LVGL
used by the embedded application to render the UI.

## Purpose

Keep the editable UI design files and related project assets together so
the dashboard interface can be reviewed and updated independently of the
firmware.

## Requirements

-   SquareLine Studio compatible with the project file and its UI/LVGL
    configuration.
-   The dashboard firmware project from this repository for integrating
    and testing exported UI code.
-   The matching LVGL version and display configuration used by the
    dashboard firmware.

> **Compatibility note:** Check the dashboard firmware project for its
> LVGL version and configuration before exporting or updating UI files.
> The LVGL version used by this project is not specified in this README.

## Opening the project

1.  Open SquareLine Studio.
2.  Choose the option to open an existing project.
3.  Select the SquareLine Studio project file included in this folder.
4.  Review the project settings and assets before making changes.

The exact project filename may vary; use the project file present in
this directory.

## Editing and integrating UI changes

1.  Make the required layout, style, screen, or asset changes in
    SquareLine Studio.
2.  Preview the UI and check that images, fonts, and other assets are
    available.
3.  Export the UI source files using the project's configured export
    options.
4.  Integrate the exported files into the dashboard firmware project,
    following its existing folder structure and build configuration.
5.  Build and test the dashboard firmware on the target CrowPanel
    display.

SquareLine Studio generates UI code; it does **not** replace the
dashboard's application logic, device communication, or firmware
configuration. Preserve the existing firmware code and configuration
when updating the generated UI.

## Assets and project files

Keep the SquareLine Studio project file and its referenced assets
together. If assets are moved or renamed, update their references in
SquareLine Studio and verify them before exporting.

Do not assume that exporting UI files alone creates a complete,
flashable firmware image. Firmware must be built using the dashboard
project's normal toolchain and configuration.

## Related project

-   Repository: [reSpeaker Smart Home AI
    Assistant](https://github.com/make2explore/reSpeaker-Smart-Home-AI-Assistant)
-   Target interface: CrowPanel Advance touchscreen dashboard
-   Related firmware: see the dashboard firmware source under the
    `Codes` directory.

## Notes

-   This folder is for the editable UI design project.
-   Use the dashboard firmware project as the source of truth for LVGL
    version, display settings, and integration paths.
-   Test UI changes on the target hardware to confirm touch interaction,
    layout, and rendering.
