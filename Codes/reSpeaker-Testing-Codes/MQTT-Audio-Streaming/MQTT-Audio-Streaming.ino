#include "WiFi.h"                // For WiFi connectivity
#include "ArduinoMqttClient.h"   // For MQTT communication
#include "AudioTools.h"          // For handling I2S audio and WAV encoding

// Define how much audio data to send
#define MQTT_PACKET_SIZE 1024    // Size of each MQTT data packet (in bytes)
#define NUM_PACKETS 375          // Number of packets to send (~3 seconds of audio at 16kHz)

// WiFi network credentials
const char* ssid = "xxxxxxx";
const char* password = "xxxxxxx";

// MQTT broker configuration
const char* broker = "test.mosquitto.org";   // Public test MQTT broker
const char* topic = "xvf3800/audio.wav";     // Topic to publish audio to
int port = 1883;                             // Standard MQTT port

// Create MQTT and WiFi clients
WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

// Configure audio: 16kHz sample rate, stereo (2 channels), 32-bit per sample
AudioInfo info(16000, 2, 32);
I2SStream i2s_in;           // I2S audio input stream
I2SConfig i2s_config;       // I2S hardware configuration

// Stream that encodes audio in WAV format and sends it via MQTT
EncodedAudioStream out_stream(&mqttClient, new WAVEncoder());
StreamCopy copier(out_stream, i2s_in, MQTT_PACKET_SIZE); // Handles copying I2S data into the MQTT stream

// Connect to WiFi network
void connectWiFi() {
  Serial.printf("Connecting to WiFi: %s\n", ssid);
  WiFi.begin(ssid, password);                // Start WiFi connection
  while (WiFi.status() != WL_CONNECTED) {    // Wait until connected
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nConnected to WiFi!");
}

// Connect to MQTT broker
void connectMQTT() {
  mqttClient.setId("xvf3800_stream");        // Set MQTT client ID
  Serial.printf("Connecting to MQTT broker: %s\n", broker);

  if (!mqttClient.connect(broker, port)) {   // Try to connect to MQTT
    Serial.print("MQTT connection failed! Code: ");
    Serial.println(mqttClient.connectError());
    while (true); // Stop here if connection fails
  }

  Serial.println("Connected to MQTT broker!");
}

// Set up I2S audio input configuration
void setupI2SInput() {
  i2s_config = i2s_in.defaultConfig(RX_MODE);  // Set RX mode for audio input
  i2s_config.copyFrom(info);                   // Apply audio format to config

  // Assign correct GPIO pins for XVF3800
  i2s_config.pin_bck = 8;       // Bit clock
  i2s_config.pin_ws = 7;        // Word select (LR clock)
  i2s_config.pin_data = 44;     // Not used for RX, but required by lib
  i2s_config.pin_data_rx = 43;  // Data input pin from XVF3800
  i2s_config.is_master = true;  // XVF3800 provides clock, so this board acts as master

  i2s_in.begin(i2s_config);     // Initialize I2S input with given config
  Serial.println("I2S input started.");
}

void setup() {
  Serial.begin(115200);         // Start serial monitor
  AudioLogger::instance().begin(Serial, AudioLogger::Info); // Enable audio debug logs

  connectWiFi();                // Connect to WiFi
  connectMQTT();                // Connect to MQTT
  setupI2SInput();              // Configure and start I2S audio input

  out_stream.begin(info);       // Initialize WAV encoder with audio format

  // Start a new MQTT message and reserve enough space for the full audio stream
  mqttClient.beginMessage(topic, MQTT_PACKET_SIZE * NUM_PACKETS, true);

  // Copy audio from microphone (I2S) into MQTT stream
  copier.copyN(NUM_PACKETS);   // Copies a fixed number of packets (3 seconds of audio)

  mqttClient.endMessage();     // Finalize the MQTT message and send it
  Serial.println("Audio stream sent via MQTT!");
}

void loop() {
  mqttClient.poll();           // Keep MQTT connection alive (important if broker expects pings)
  delay(5000);                 // Optional delay, useful if recording repeatedly
}
