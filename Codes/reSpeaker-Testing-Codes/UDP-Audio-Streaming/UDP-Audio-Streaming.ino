#include "WiFi.h"
#include "WiFiUdp.h"
#include "AudioTools.h"

// WiFi credentials
const char* ssid = "xxxxxx";
const char* password = "xxxxxxx";

// UDP target
const char* udpAddress = "192.168.0.5";
const int udpPort = 12345;

WiFiUDP udp;

// Audio: 16kHz, stereo, 32-bit
AudioInfo info(16000, 2, 32);
I2SStream i2s_in;
I2SConfig i2s_config;

// Buffer for reading audio
#define BUFFER_SIZE 1024
uint8_t buffer[BUFFER_SIZE];

// 5 seconds of audio = 16000 Hz × 2 channels × 4 bytes = 128,000 bytes/sec
// 5 seconds = 640,000 bytes
#define TOTAL_BYTES 640000

void connectWiFi() {
  Serial.printf("Connecting to WiFi: %s\n", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected!");
  Serial.printf("IP Address: %s\n", WiFi.localIP().toString().c_str());
}

void setupI2SInput() {
  i2s_config = i2s_in.defaultConfig(RX_MODE);
  i2s_config.copyFrom(info);
  
  // XVF3800 pins
  i2s_config.pin_bck = 8;
  i2s_config.pin_ws = 7;
  i2s_config.pin_data = 44;
  i2s_config.pin_data_rx = 43;
  i2s_config.is_master = true;
  
  i2s_in.begin(i2s_config);
  Serial.println("I2S input started.");
}

void setup() {
  Serial.begin(115200);
  while(!Serial);
  
  AudioLogger::instance().begin(Serial, AudioLogger::Info);
  
  connectWiFi();
  setupI2SInput();
  
  // Wait a bit for I2S to stabilize
  delay(500);
  
  Serial.printf("Sending 5 seconds of audio via UDP to %s:%d\n", udpAddress, udpPort);
  
  size_t total_sent = 0;
  size_t bytes_read = 0;
  
  // Send audio in chunks
  while (total_sent < TOTAL_BYTES) {
    // Read audio data
    bytes_read = i2s_in.readBytes(buffer, BUFFER_SIZE);
    
    if (bytes_read > 0) {
      // Send via UDP
      udp.beginPacket(udpAddress, udpPort);
      udp.write(buffer, bytes_read);
      udp.endPacket();
      
      total_sent += bytes_read;
      
      // Progress indicator
      if (total_sent % 64000 == 0) {
        Serial.printf("Sent %d bytes (%.1f seconds)\n", total_sent, total_sent / 128000.0);
      }
    } else {
      Serial.println("Warning: No data read from I2S");
      delay(10);
    }
  }
  
  Serial.printf("Finished! Sent %d bytes total\n", total_sent);
}

void loop() {
  // Nothing - runs once
}