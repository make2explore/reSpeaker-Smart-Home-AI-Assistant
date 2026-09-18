#include "WiFi.h"
#include "HTTPClient.h"
#include "AudioTools.h"

// WiFi credentials
const char* ssid = "xxxxxx";
const char* password = "xxxxxxx";

// HTTP server (your PC)
const char* serverUrl = "http://192.168.0.5:8000/upload"; // please replace with your PC IP /flask server 

// Audio: 16kHz, stereo, 32-bit
AudioInfo info(16000, 2, 32);
I2SStream i2s_in;
I2SConfig i2s_config;

// 5 seconds of audio = 640,000 bytes
#define RECORDING_SECONDS 5
#define BYTES_PER_SECOND 128000
#define TOTAL_BYTES (RECORDING_SECONDS * BYTES_PER_SECOND)

// Buffer for recording
uint8_t* audioBuffer = nullptr;

void connectWiFi() {
  Serial.printf("Connecting to WiFi: %s\n", ssid);
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected!");
    Serial.printf("IP Address: %s\n", WiFi.localIP().toString().c_str());
  } else {
    Serial.println("\nFailed to connect!");
  }
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
  
  // Allocate memory for audio buffer
  Serial.printf("Free heap: %u\n", ESP.getFreeHeap());
  Serial.printf("Largest free block: %u\n", ESP.getMaxAllocHeap());

  audioBuffer = (uint8_t*)malloc(TOTAL_BYTES);
  if (!audioBuffer) {
    Serial.println("Failed to allocate memory!");
    return;
  }
  
  connectWiFi();
  setupI2SInput();
  
  // Wait for I2S to stabilize
  delay(500);
  
  // Record audio
  Serial.printf("Recording %d seconds of audio...\n", RECORDING_SECONDS);
  size_t total_read = 0;
  size_t bytes_read = 0;
  
  unsigned long start_time = millis();
  
  while (total_read < TOTAL_BYTES) {
    bytes_read = i2s_in.readBytes(audioBuffer + total_read, 
                                   min(4096, (int)(TOTAL_BYTES - total_read)));
    
    if (bytes_read > 0) {
      total_read += bytes_read;
      
      // Progress indicator
      if (total_read % BYTES_PER_SECOND == 0) {
        Serial.printf("Recorded %.1f seconds\n", total_read / (float)BYTES_PER_SECOND);
      }
    }
  }
  
  unsigned long record_time = millis() - start_time;
  Serial.printf("Recording complete! %d bytes in %lu ms\n", total_read, record_time);
  
  // Send via HTTP POST
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    
    Serial.printf("Sending audio to %s\n", serverUrl);
    
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/octet-stream");
    http.addHeader("X-Sample-Rate", String(info.sample_rate));
    http.addHeader("X-Channels", String(info.channels));
    http.addHeader("X-Bits-Per-Sample", String(info.bits_per_sample));
    
    int httpResponseCode = http.POST(audioBuffer, total_read);
    
    if (httpResponseCode > 0) {
      Serial.printf("HTTP Response code: %d\n", httpResponseCode);
      String response = http.getString();
      Serial.println("Response: " + response);
    } else {
      Serial.printf("Error code: %d\n", httpResponseCode);
      Serial.println("Error: " + http.errorToString(httpResponseCode));
    }
    
    http.end();
  } else {
    Serial.println("WiFi not connected!");
  }
  
  // Free memory
  free(audioBuffer);
  
  Serial.println("Done!");
}

void loop() {
  // Nothing - runs once
}
