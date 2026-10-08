// ---------------------------------- make2explore.com -------------------------------------------------------//
// Project           - reSpeaker Smart Home AI Assistant
// Created By        - info@make2explore.com
// Last Modified     - 08/10/2026 20:11:00 @admin
// Software          - C/C++, Arduino IDE, Libraries - Arduino library for Sensirion SHT4x sensors
// Hardware          - XIAO ESP32-S3, Grove - Temperature & Humidity Sensor (SHT40), 4CH Relay Board (5V)     
// Sensors Used      - Grove - Temperature & Humidity Sensor (SHT40)
// Source Repo       - github.com/make2explore
// ===========================================================================================================//
// This is Home Controller code for XIAO ESP32-S3

#include <WiFi.h>
#include <Preferences.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <SensirionI2cSht4x.h>
#include <Wire.h>



// ==================== Wi-Fi Provisioning ====================

Preferences preferences;



String wifiSSID;

String wifiPassword;



bool provisioningMode = false;



const char* PROVISIONING_AP_PREFIX = "Home-Controller-Setup";

const char* PROVISIONING_AP_PASSWORD = "setup1234";



const unsigned long WIFI_CONNECT_TIMEOUT = 15000;





// ==================== Wi-Fi Provisioning Functions ====================



// ============================================================
// Web servers
// ============================================================

WebServer server(80);
WebServer provisioningServer(80);
String provisioningAPSSID;

void loadWiFiCredentials() {

    preferences.begin("wifi", true);



    wifiSSID = preferences.getString("ssid", "");

    wifiPassword = preferences.getString("password", "");



    preferences.end();



    Serial.println();

    Serial.println("Wi-Fi credentials loaded from NVS.");



    if (wifiSSID.length() > 0) {

        Serial.print("Saved SSID: ");

        Serial.println(wifiSSID);

    } else {

        Serial.println("No saved Wi-Fi credentials found.");

    }

}



void saveWiFiCredentials(const String& ssid, const String& password) {

    preferences.begin("wifi", false);



    preferences.putString("ssid", ssid);

    preferences.putString("password", password);



    preferences.end();



    wifiSSID = ssid;

    wifiPassword = password;



    Serial.println("Wi-Fi credentials saved to NVS.");

}



bool connectToWiFi(const String& ssid, const String& password) {

    Serial.println();

    Serial.print("Connecting to Wi-Fi: ");

    Serial.println(ssid);



    WiFi.mode(WIFI_STA);

    WiFi.begin(ssid.c_str(), password.c_str());



    unsigned long startTime = millis();



    while (WiFi.status() != WL_CONNECTED &&

           millis() - startTime < WIFI_CONNECT_TIMEOUT) {

        delay(500);

        Serial.print(".");

    }



    Serial.println();



    if (WiFi.status() == WL_CONNECTED) {

        Serial.println("Wi-Fi connected successfully.");

        Serial.print("IP address: ");

        Serial.println(WiFi.localIP());

        return true;

    }



    Serial.println("Wi-Fi connection failed.");

    WiFi.disconnect();

    return false;

}




// Test new credentials without shutting down the provisioning AP.
// The setup AP remains available so the user can retry after a
// wrong password or other connection failure.
bool testProvisioningWiFi(const String& ssid, const String& password) {

    Serial.println();
    Serial.print("Testing Wi-Fi credentials: ");
    Serial.println(ssid);

    // Keep the provisioning SoftAP alive while testing.
    WiFi.mode(WIFI_AP_STA);
    WiFi.begin(ssid.c_str(), password.c_str());

    unsigned long startTime = millis();

    while (WiFi.status() != WL_CONNECTED &&
           millis() - startTime < WIFI_CONNECT_TIMEOUT) {
        delay(500);
        Serial.print(".");
    }

    Serial.println();

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("Provisioning Wi-Fi credentials are valid.");
        Serial.print("Station IP address: ");
        Serial.println(WiFi.localIP());
        return true;
    }

    Serial.println("Provisioning Wi-Fi credentials failed.");

    // Disconnect only the failed station connection.
    // Keep WIFI_AP_STA so the provisioning AP remains available.
    WiFi.disconnect(false);
    delay(100);
    WiFi.mode(WIFI_AP_STA);

    return false;
}


void startProvisioningMode() {

    provisioningMode = true;



    uint8_t mac[6];

    WiFi.macAddress(mac);



    char suffix[5];

    snprintf(suffix, sizeof(suffix), "%02X%02X", mac[4], mac[5]);



    provisioningAPSSID = String(PROVISIONING_AP_PREFIX) + "-" + suffix;



    WiFi.mode(WIFI_AP_STA);



    bool apStarted = WiFi.softAP(

        provisioningAPSSID.c_str(),

        PROVISIONING_AP_PASSWORD

    );



    if (apStarted) {

        Serial.println();

        Serial.println("========================================");

        Serial.println("Wi-Fi Provisioning Mode");

        Serial.println("========================================");

        Serial.print("AP SSID: ");

        Serial.println(provisioningAPSSID);

        Serial.print("AP Password: ");

        Serial.println(PROVISIONING_AP_PASSWORD);

        Serial.print("Setup page: http://");

        Serial.println(WiFi.softAPIP());

        Serial.println("========================================");

    } else {

        Serial.println("Failed to start provisioning AP.");

    }

}



String getProvisioningPage() {

    String html = R"rawliteral(

<!DOCTYPE html>

<html>

<head>

    <meta name="viewport" content="width=device-width, initial-scale=1">

    <title>Home Controller Setup</title>

    <style>

        body {

            font-family: Arial, sans-serif;

            background: #f2f2f2;

            margin: 0;

            padding: 20px;

        }



        .container {

            max-width: 420px;

            margin: auto;

            background: white;

            padding: 24px;

            border-radius: 12px;

            box-shadow: 0 2px 10px rgba(0,0,0,0.15);

        }



        h2 {

            margin-top: 0;

        }



        label {

            display: block;

            margin-top: 16px;

            margin-bottom: 6px;

            font-weight: bold;

        }



        select,

        input,

        button {

            width: 100%;

            box-sizing: border-box;

            padding: 12px;

            font-size: 16px;

            border-radius: 6px;

            border: 1px solid #ccc;

        }



        button {

            margin-top: 18px;

            background: #1976d2;

            color: white;

            border: none;

            cursor: pointer;

        }



        button:active {

            background: #125a9c;

        }



        .info {

            margin-bottom: 20px;

            color: #555;

        }



        .password-row {
            position: relative;
            width: 100%;
        }

        .password-row input {
            width: 100%;
            box-sizing: border-box;
            padding-right: 52px;
        }

        .password-eye {
            position: absolute;
            right: 10px;
            top: 50%;
            transform: translateY(-50%);
            width: 36px;
            height: 36px;
            margin: 0;
            padding: 0;
            border: none;
            background: transparent;
            color: #555;
            cursor: pointer;
            display: flex;
            align-items: center;
            justify-content: center;
        }

        .password-eye:active {
            background: transparent;
        }

        .password-eye svg {
            width: 24px;
            height: 24px;
            pointer-events: none;
        }

        #status {

            margin-top: 18px;

            font-weight: bold;

        }

    </style>

</head>



<body>

<div class="container">



    <h2>Home Controller Setup</h2>



    <div class="info">

        Connect the Home Controller to your Wi-Fi network.

    </div>



    <label for="network">Wi-Fi Network</label>

    <select id="network">

        <option value="">Select network</option>

    </select>



    <button onclick="scanNetworks()">Scan Networks</button>



    <label for="password">Wi-Fi Password</label>

    <div class="password-row">
        <input type="password" id="password" placeholder="Enter Wi-Fi password">
        <button type="button" class="password-eye" onclick="togglePassword()" id="passwordToggle" aria-label="Show password">
            <svg id="eyeIcon" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" aria-hidden="true">
                <path d="M2 12s3.5-7 10-7 10 7 10 7-3.5 7-10 7S2 12 2 12z"></path>
                <circle cx="12" cy="12" r="3"></circle>
            </svg>
        </button>
    </div>



    <button onclick="connectWiFi()">Connect</button>



    <div id="status"></div>



</div>



<script>

function togglePassword() {
    const password = document.getElementById('password');
    const button = document.getElementById('passwordToggle');
    const eyeIcon = document.getElementById('eyeIcon');

    if (password.type === 'password') {
        password.type = 'text';
        button.setAttribute('aria-label', 'Hide password');
        eyeIcon.innerHTML =
            '<path d="M3 3l18 18"></path>' +
            '<path d="M10.6 10.6a2 2 0 0 0 2.8 2.8"></path>' +
            '<path d="M9.9 5.2A10.7 10.7 0 0 1 12 5c6.5 0 10 7 10 7a18.3 18.3 0 0 1-3.2 4.1"></path>' +
            '<path d="M6.2 6.2C3.5 8.1 2 12 2 12s3.5 7 10 7c1.4 0 2.7-.3 3.9-.8"></path>';
    } else {
        password.type = 'password';
        button.setAttribute('aria-label', 'Show password');
        eyeIcon.innerHTML =
            '<path d="M2 12s3.5-7 10-7 10 7 10 7-3.5 7-10 7S2 12 2 12z"></path>' +
            '<circle cx="12" cy="12" r="3"></circle>';
    }
}

function scanNetworks() {

    const status = document.getElementById("status");

    status.innerHTML = "Scanning for Wi-Fi networks...";



    fetch("/scan")

        .then(response => response.json())

        .then(networks => {

            const select = document.getElementById("network");



            select.innerHTML =

                '<option value="">Select network</option>';



            networks.forEach(network => {

                const option = document.createElement("option");



                option.value = network.ssid;



                option.textContent =

                    network.ssid +

                    (network.secured ? " (Secured)" : " (Open)");



                select.appendChild(option);

            });



            status.innerHTML =

                "Found " + networks.length + " network(s).";

        })

        .catch(error => {

            status.innerHTML =

                "Unable to scan for networks.";

        });

}



function connectWiFi() {

    const ssid =

        document.getElementById("network").value;



    const password =

        document.getElementById("password").value;



    const status =

        document.getElementById("status");



    if (!ssid) {

        status.innerHTML =

            "Please select a Wi-Fi network.";

        return;

    }



    status.innerHTML =

        "Testing Wi-Fi connection...";



    fetch("/save", {
        method: "POST",
        headers: { "Content-Type": "application/x-www-form-urlencoded" },
        body: "ssid=" + encodeURIComponent(ssid) +
              "&password=" + encodeURIComponent(password)
    })

    .then(response => response.text())

    .then(result => {

        status.innerHTML = result;

    })

    .catch(error => {

        status.innerHTML =

            "Connection request failed.";

    });

}

</script>



</body>

</html>

)rawliteral";



    return html;

}



void handleProvisioningScan() {

    Serial.println("Scanning for Wi-Fi networks...");



    int networkCount = WiFi.scanNetworks();



    String json = "[";



    for (int i = 0; i < networkCount; i++) {

        if (i > 0) {

            json += ",";

        }



        json += "{\"ssid\":\"";

        json += WiFi.SSID(i);

        json += "\",\"secured\":";

        json += (WiFi.encryptionType(i) != WIFI_AUTH_OPEN) ? "true" : "false";

        json += "}";

    }



    json += "]";



    WiFi.scanDelete();



    provisioningServer.send(

        200,

        "application/json",

        json

    );



    Serial.print("Found ");

    Serial.print(networkCount);

    Serial.println(" Wi-Fi network(s).");

}



void handleProvisioningSave() {

    String newSSID = provisioningServer.arg("ssid");
    String newPassword = provisioningServer.arg("password");

    if (newSSID.length() == 0) {
        provisioningServer.send(
            400,
            "text/plain",
            "Please select a Wi-Fi network."
        );
        return;
    }

    Serial.println();
    Serial.println("Testing new Wi-Fi credentials...");
    Serial.print("SSID: ");
    Serial.println(newSSID);

    // Test while keeping the provisioning AP alive.
    if (testProvisioningWiFi(newSSID, newPassword)) {

        saveWiFiCredentials(newSSID, newPassword);

        // Send success while the setup AP is still alive.
        provisioningServer.send(
            200,
            "text/plain",
            "Wi-Fi connected successfully. Credentials saved. The Home Controller is switching to normal Wi-Fi mode."
        );

        delay(1000);

        // Leave provisioning mode ONLY after successful credentials.
        provisioningServer.stop();
        WiFi.softAPdisconnect(true);

        provisioningMode = false;

        if (!MDNS.begin("home-controller")) {
            Serial.println("Error starting mDNS after provisioning.");
        } else {
            Serial.println("mDNS started: http://home-controller.local");
        }

        server.begin();
        Serial.println("Home Controller HTTP server started.");

        Serial.println("Provisioning completed.");
        Serial.println("Home Controller is now running in normal Wi-Fi mode.");

    } else {

        // Failed credentials are NOT saved.
        // The setup AP remains active for an immediate retry.
        provisioningServer.send(
            200,
            "text/plain",
            "Incorrect Wi-Fi password or connection failed. The Home Controller is still in setup mode. Please check the password and try again."
        );

        Serial.println("Provisioning attempt failed. Setup AP remains active.");
    }
}


void setupProvisioningServer() {

    provisioningServer.on("/", HTTP_GET, []() {

        provisioningServer.send(

            200,

            "text/html",

            getProvisioningPage()

        );

    });



    provisioningServer.on("/scan", HTTP_GET, handleProvisioningScan);



    provisioningServer.on("/save", HTTP_POST, handleProvisioningSave);



    provisioningServer.begin();



    Serial.println("Provisioning web server started.");

}



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



  loadWiFiCredentials();



  if (wifiSSID.length() > 0) {

      if (!connectToWiFi(wifiSSID, wifiPassword)) {

          Serial.println("Saved Wi-Fi credentials failed.");

          startProvisioningMode();

          setupProvisioningServer();

      }

  } else {

      Serial.println("No saved Wi-Fi credentials.");

      startProvisioningMode();

      setupProvisioningServer();

  }



  // ----------------------------------------------------------

  // mDNS
  // ----------------------------------------------------------

  if (!provisioningMode) {
    if (!MDNS.begin("home-controller")) {
      Serial.println("Error starting mDNS");
    } else {
      Serial.println("mDNS started: http://home-controller.local");
    }
  }

  // ----------------------------------------------------------
  // Root// Root

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



  if (!provisioningMode) {

      server.begin();



      Serial.println("Home Controller HTTP server started.");

  }

}



// ============================================================

// Loop

// ============================================================



void loop()

{

  if (!provisioningMode) {
    server.handleClient();
  }



  if (provisioningMode) {

    provisioningServer.handleClient();

  }



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