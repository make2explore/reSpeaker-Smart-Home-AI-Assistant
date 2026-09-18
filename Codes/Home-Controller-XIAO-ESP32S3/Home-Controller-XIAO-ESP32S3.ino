// ---------------------------------- make2explore.com -------------------------------------------------------//
// Project           - reSpeaker Smart Home AI Assistant
// Created By        - info@make2explore.com
// Last Modified     - 18/09/2026 20:11:00 @admin
// Software          - C/C++, Arduino IDE, Libraries - Arduino library for Sensirion SHT4x sensors
// Hardware          - XIAO ESP32-S3, Grove - Temperature & Humidity Sensor (SHT40), 4CH Relay Board (5V)     
// Sensors Used      - Grove - Temperature & Humidity Sensor (SHT40)
// Source Repo       - github.com/make2explore
// ===========================================================================================================//
// This is Home Controller code for XIAO ESP32-S3

#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <SensirionI2cSht4x.h>
#include <Wire.h>

// ============================================================
// Wi-Fi (Relplace with your Wi-Fi Credentials)
// ============================================================

const char* ssid = "xxxxxxx";
const char* password = "xxxxxxx";

// ============================================================
// XIAO ESP32-S3 relay GPIOs
//
// D0 = GPIO1 -> Light
// D1 = GPIO2 -> AC
// D2 = GPIO3 -> Fan
// D3 = GPIO4 -> TV
//
// Relay module:
// LOW  = ON
// HIGH = OFF
// ============================================================

const int RELAY_LIGHT = 1;
const int RELAY_AC    = 2;
const int RELAY_FAN   = 3;
const int RELAY_TV    = 4;

// ============================================================
// SHT40
//
// XIAO ESP32-S3 I2C:
// D4 / GPIO5 -> SDA
// D5 / GPIO6 -> SCL
//
// SHT40 default I2C address = 0x44
// ============================================================

SensirionI2cSht4x sensor;

static char shtErrorMessage[64];
static int16_t shtError;

// Latest sensor readings
float currentTemperature = 0.0;
float currentHumidity = 0.0;

// Sensor reading interval
unsigned long lastSensorRead = 0;
const unsigned long SENSOR_READ_INTERVAL = 5000;

// ============================================================
// Web server
// ============================================================

WebServer server(80);

// ============================================================
// Device state
// ============================================================

bool lightState = false;
bool acState    = false;
bool fanState   = false;
bool tvState    = false;

// ============================================================
// Relay control
// ============================================================

void setRelay(int pin, bool on)
{
  // Active-low relay
  digitalWrite(pin, on ? LOW : HIGH);
}

// ============================================================
// Device state control
// ============================================================

bool setDeviceState(const char* device, bool on)
{
  if (strcmp(device, "light") == 0) {
    lightState = on;
    setRelay(RELAY_LIGHT, on);
    return true;
  }

  if (strcmp(device, "ac") == 0) {
    acState = on;
    setRelay(RELAY_AC, on);
    return true;
  }

  if (strcmp(device, "fan") == 0) {
    fanState = on;
    setRelay(RELAY_FAN, on);
    return true;
  }

  if (strcmp(device, "tv") == 0) {
    tvState = on;
    setRelay(RELAY_TV, on);
    return true;
  }

  return false;
}

bool getDeviceState(const char* device)
{
  if (strcmp(device, "light") == 0) {
    return lightState;
  }

  if (strcmp(device, "ac") == 0) {
    return acState;
  }

  if (strcmp(device, "fan") == 0) {
    return fanState;
  }

  if (strcmp(device, "tv") == 0) {
    return tvState;
  }

  return false;
}

// ============================================================
// JSON response helpers
// ============================================================

void sendDeviceStateResponse(
    const char* device,
    bool on,
    bool includeDevice)
{
  String json;

  if (includeDevice) {
    json = "{\"success\":true,\"device\":\"";
    json += device;
    json += "\",\"state\":\"";
  } else {
    json = "{\"success\":true,\"state\":\"";
  }

  json += (on ? "on" : "off");
  json += "\"}";

  server.send(
      200,
      "application/json",
      json);
}

void sendDeviceStatusResponse(const char* device)
{
  bool state = getDeviceState(device);

  String json =
      "{\"success\":true,\"device\":\"";

  json += device;
  json += "\",\"state\":\"";
  json += (state ? "on" : "off");
  json += "\"}";

  server.send(
      200,
      "application/json",
      json);
}

// ============================================================
// SHT40 - Read sensor
// ============================================================

void readSHT40()
{
  float temperature = 0.0;
  float humidity = 0.0;

  delay(20);

  shtError = sensor.measureHighPrecision(
      temperature,
      humidity);

  if (shtError != 0) {

    Serial.print(
        "SHT40 measurement error: ");

    errorToString(
        shtError,
        shtErrorMessage,
        sizeof shtErrorMessage);

    Serial.println(shtErrorMessage);

    return;
  }

  currentTemperature = temperature;
  currentHumidity = humidity;

  Serial.print("SHT40 Temperature: ");
  Serial.print(currentTemperature, 2);
  Serial.println(" °C");

  Serial.print("SHT40 Humidity:    ");
  Serial.print(currentHumidity, 2);
  Serial.println(" %RH");
}

// ============================================================
// Environment API
//
// GET /api/environment
//
// Example response:
//
// {
//   "success": true,
//   "temperature": 29.96,
//   "humidity": 79.41
// }
// ============================================================

void handleEnvironment()
{
  String json = "{\"success\":true";

  json += ",\"temperature\":";
  json += String(currentTemperature, 2);

  json += ",\"humidity\":";
  json += String(currentHumidity, 2);

  json += "}";

  server.send(
      200,
      "application/json",
      json);
}

// ============================================================
// Root
// ============================================================

void handleRoot()
{
  server.send(
      200,
      "text/plain",
      "XIAO ESP32-S3 Smart Home Controller is running");
}

// ============================================================
// LIGHT - Legacy API
// ============================================================

void handleLightOn()
{
  setDeviceState("light", true);

  sendDeviceStateResponse(
      "light",
      true,
      false);

  Serial.println("Light: ON");
}

void handleLightOff()
{
  setDeviceState("light", false);

  sendDeviceStateResponse(
      "light",
      false,
      false);

  Serial.println("Light: OFF");
}

void handleLightStatus()
{
  sendDeviceStatusResponse("light");
}

// ============================================================
// AC - Legacy API
// ============================================================

void handleAcOn()
{
  setDeviceState("ac", true);

  sendDeviceStateResponse(
      "ac",
      true,
      false);

  Serial.println("AC: ON");
}

void handleAcOff()
{
  setDeviceState("ac", false);

  sendDeviceStateResponse(
      "ac",
      false,
      false);

  Serial.println("AC: OFF");
}

void handleAcStatus()
{
  sendDeviceStatusResponse("ac");
}

// ============================================================
// FAN - Legacy API
// ============================================================

void handleFanOn()
{
  setDeviceState("fan", true);

  sendDeviceStateResponse(
      "fan",
      true,
      false);

  Serial.println("Fan: ON");
}

void handleFanOff()
{
  setDeviceState("fan", false);

  sendDeviceStateResponse(
      "fan",
      false,
      false);

  Serial.println("Fan: OFF");
}

void handleFanStatus()
{
  sendDeviceStatusResponse("fan");
}

// ============================================================
// TV - Legacy API
// ============================================================

void handleTvOn()
{
  setDeviceState("tv", true);

  sendDeviceStateResponse(
      "tv",
      true,
      false);

  Serial.println("TV: ON");
}

void handleTvOff()
{
  setDeviceState("tv", false);

  sendDeviceStateResponse(
      "tv",
      false,
      false);

  Serial.println("TV: OFF");
}

void handleTvStatus()
{
  sendDeviceStatusResponse("tv");
}

// ============================================================
// LIGHT - /api/device/... aliases
// ============================================================

void handleDeviceLightOn()
{
  setDeviceState("light", true);

  sendDeviceStateResponse(
      "light",
      true,
      true);

  Serial.println("Light: ON");
}

void handleDeviceLightOff()
{
  setDeviceState("light", false);

  sendDeviceStateResponse(
      "light",
      false,
      true);

  Serial.println("Light: OFF");
}

void handleDeviceLightStatus()
{
  sendDeviceStatusResponse("light");
}

// ============================================================
// AC - /api/device/... aliases
// ============================================================

void handleDeviceAcOn()
{
  setDeviceState("ac", true);

  sendDeviceStateResponse(
      "ac",
      true,
      true);

  Serial.println("AC: ON");
}

void handleDeviceAcOff()
{
  setDeviceState("ac", false);

  sendDeviceStateResponse(
      "ac",
      false,
      true);

  Serial.println("AC: OFF");
}

void handleDeviceAcStatus()
{
  sendDeviceStatusResponse("ac");
}

// ============================================================
// FAN - /api/device/... aliases
// ============================================================

void handleDeviceFanOn()
{
  setDeviceState("fan", true);

  sendDeviceStateResponse(
      "fan",
      true,
      true);

  Serial.println("Fan: ON");
}

void handleDeviceFanOff()
{
  setDeviceState("fan", false);

  sendDeviceStateResponse(
      "fan",
      false,
      true);

  Serial.println("Fan: OFF");
}

void handleDeviceFanStatus()
{
  sendDeviceStatusResponse("fan");
}

// ============================================================
// TV - /api/device/... aliases
// ============================================================

void handleDeviceTvOn()
{
  setDeviceState("tv", true);

  sendDeviceStateResponse(
      "tv",
      true,
      true);

  Serial.println("TV: ON");
}

void handleDeviceTvOff()
{
  setDeviceState("tv", false);

  sendDeviceStateResponse(
      "tv",
      false,
      true);

  Serial.println("TV: OFF");
}

void handleDeviceTvStatus()
{
  sendDeviceStatusResponse("tv");
}

// ============================================================
// All-device status
// ============================================================

void handleAllDevicesStatus()
{
  String json = "{\"success\":true";

  json += ",\"light\":\"";
  json += (lightState ? "on" : "off");

  json += "\",\"ac\":\"";
  json += (acState ? "on" : "off");

  json += "\",\"fan\":\"";
  json += (fanState ? "on" : "off");

  json += "\",\"tv\":\"";
  json += (tvState ? "on" : "off");

  json += "\"}";

  server.send(
      200,
      "application/json",
      json);
}

// ============================================================
// Setup
// ============================================================

void setup()
{
  Serial.begin(115200);

  // ----------------------------------------------------------
  // Relay outputs
  // ----------------------------------------------------------

  pinMode(RELAY_LIGHT, OUTPUT);
  pinMode(RELAY_AC, OUTPUT);
  pinMode(RELAY_FAN, OUTPUT);
  pinMode(RELAY_TV, OUTPUT);

  // Always OFF after reboot / power restoration
  digitalWrite(RELAY_LIGHT, HIGH);
  digitalWrite(RELAY_AC, HIGH);
  digitalWrite(RELAY_FAN, HIGH);
  digitalWrite(RELAY_TV, HIGH);

  // Reset software states
  lightState = false;
  acState = false;
  fanState = false;
  tvState = false;

  // ----------------------------------------------------------
  // I2C / SHT40
  // ----------------------------------------------------------

  Wire.begin();

  sensor.begin(
      Wire,
      SHT40_I2C_ADDR_44);

  sensor.softReset();

  delay(10);

  uint32_t serialNumber = 0;

  shtError = sensor.serialNumber(
      serialNumber);

  if (shtError != 0) {

    Serial.print(
        "SHT40 initialization error: ");

    errorToString(
        shtError,
        shtErrorMessage,
        sizeof shtErrorMessage);

    Serial.println(shtErrorMessage);

  } else {

    Serial.print(
        "SHT40 detected. Serial Number: ");

    Serial.println(serialNumber);

    // Get first reading
    readSHT40();
  }

  // ----------------------------------------------------------
  // Wi-Fi
  // ----------------------------------------------------------

  WiFi.begin(ssid, password);

  Serial.print("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Wi-Fi connected!");

  Serial.print("XIAO ESP32-S3 IP address: ");
  Serial.println(WiFi.localIP());

  // ----------------------------------------------------------
  // mDNS
  // ----------------------------------------------------------

  if (!MDNS.begin("home-controller")) {

    Serial.println(
        "Error starting mDNS");

  } else {

    Serial.println(
        "mDNS started: http://home-controller.local");
  }

  // ----------------------------------------------------------
  // Root
  // ----------------------------------------------------------

  server.on(
      "/",
      HTTP_GET,
      handleRoot);

  // ==========================================================
  // LEGACY API
  // ==========================================================

  // --------------------------
  // Light
  // --------------------------

  server.on(
      "/api/light/on",
      HTTP_POST,
      handleLightOn);

  server.on(
      "/api/light/off",
      HTTP_POST,
      handleLightOff);

  server.on(
      "/api/light/status",
      HTTP_GET,
      handleLightStatus);

  // --------------------------
  // AC
  // --------------------------

  server.on(
      "/api/ac/on",
      HTTP_POST,
      handleAcOn);

  server.on(
      "/api/ac/off",
      HTTP_POST,
      handleAcOff);

  server.on(
      "/api/ac/status",
      HTTP_GET,
      handleAcStatus);

  // --------------------------
  // Fan
  // --------------------------

  server.on(
      "/api/fan/on",
      HTTP_POST,
      handleFanOn);

  server.on(
      "/api/fan/off",
      HTTP_POST,
      handleFanOff);

  server.on(
      "/api/fan/status",
      HTTP_GET,
      handleFanStatus);

  // --------------------------
  // TV
  // --------------------------

  server.on(
      "/api/tv/on",
      HTTP_POST,
      handleTvOn);

  server.on(
      "/api/tv/off",
      HTTP_POST,
      handleTvOff);

  server.on(
      "/api/tv/status",
      HTTP_GET,
      handleTvStatus);

  // ==========================================================
  // /api/device/... API
  // ==========================================================

  // --------------------------
  // Light
  // --------------------------

  server.on(
      "/api/device/light/on",
      HTTP_POST,
      handleDeviceLightOn);

  server.on(
      "/api/device/light/off",
      HTTP_POST,
      handleDeviceLightOff);

  server.on(
      "/api/device/light/status",
      HTTP_GET,
      handleDeviceLightStatus);

  // --------------------------
  // AC
  // --------------------------

  server.on(
      "/api/device/ac/on",
      HTTP_POST,
      handleDeviceAcOn);

  server.on(
      "/api/device/ac/off",
      HTTP_POST,
      handleDeviceAcOff);

  server.on(
      "/api/device/ac/status",
      HTTP_GET,
      handleDeviceAcStatus);

  // --------------------------
  // Fan
  // --------------------------

  server.on(
      "/api/device/fan/on",
      HTTP_POST,
      handleDeviceFanOn);

  server.on(
      "/api/device/fan/off",
      HTTP_POST,
      handleDeviceFanOff);

  server.on(
      "/api/device/fan/status",
      HTTP_GET,
      handleDeviceFanStatus);

  // --------------------------
  // TV
  // --------------------------

  server.on(
      "/api/device/tv/on",
      HTTP_POST,
      handleDeviceTvOn);

  server.on(
      "/api/device/tv/off",
      HTTP_POST,
      handleDeviceTvOff);

  server.on(
      "/api/device/tv/status",
      HTTP_GET,
      handleDeviceTvStatus);

  // ==========================================================
  // Combined status
  // ==========================================================

  server.on(
      "/api/devices/status",
      HTTP_GET,
      handleAllDevicesStatus);

  // ==========================================================
  // Environment
  // ==========================================================

  server.on(
      "/api/environment",
      HTTP_GET,
      handleEnvironment);

  // ----------------------------------------------------------

  server.begin();

  Serial.println("HTTP server started");
}

// ============================================================
// Loop
// ============================================================

void loop()
{
  server.handleClient();

  // ----------------------------------------------------------
  // Periodic SHT40 reading
  // ----------------------------------------------------------

  unsigned long currentMillis = millis();

  if (currentMillis - lastSensorRead >=
      SENSOR_READ_INTERVAL) {

    lastSensorRead = currentMillis;

    readSHT40();
  }
}