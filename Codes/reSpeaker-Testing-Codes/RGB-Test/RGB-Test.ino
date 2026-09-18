#include <Wire.h>

#define XMOS_ADDR 0x2C
#define GPO_SERVICER_RESID 20

#define GPO_SERVICER_RESID_LED_EFFECT 12
#define GPO_SERVICER_RESID_LED_COLOR 16
#define GPO_SERVICER_RESID_LED_SPEED 15
#define GPO_SERVICER_RESID_LED_BRIGHTNESS 13

void setup() {
  Wire.begin();
  Serial.begin(115200);
  delay(1000);

  setLEDEffect(1);        // LED effect ID 1
  setLEDColor(0xFF8800);  // LED color: orange (24-bit RGB)
  setLEDSpeed(1);         // Speed: 1
  setLEDBrightness(255);  // Brightness: max
}

void loop() {
  // No repeating actions needed
}

void xmos_write_bytes(uint8_t resid, uint8_t cmd, uint8_t *value, uint8_t write_byte_num) {
  Wire.beginTransmission(XMOS_ADDR);
  Wire.write(resid);
  Wire.write(cmd);
  Wire.write(write_byte_num);
  for (uint8_t i = 0; i < write_byte_num; i++) {
    Wire.write(value[i]);
  }
  Wire.endTransmission();
}

void setLEDEffect(uint8_t effect) {
  uint8_t payload[1] = { effect };
  xmos_write_bytes(GPO_SERVICER_RESID, GPO_SERVICER_RESID_LED_EFFECT, payload, 1);
  Serial.println("LED effect set.");
}

void setLEDColor(uint32_t color) {
  uint8_t payload[4] = {
    (uint8_t)(color & 0xFF),         // Red
    (uint8_t)((color >> 8) & 0xFF),  // Green
    (uint8_t)((color >> 16) & 0xFF), // Blue
    0x00                             // Reserved (some systems may expect 4 bytes)
  };
  xmos_write_bytes(GPO_SERVICER_RESID, GPO_SERVICER_RESID_LED_COLOR, payload, 4);
  Serial.println("LED color set.");
}

void setLEDSpeed(uint8_t speed) {
  uint8_t payload[1] = { speed };
  xmos_write_bytes(GPO_SERVICER_RESID, GPO_SERVICER_RESID_LED_SPEED, payload, 1);
  Serial.println("LED speed set.");
}

void setLEDBrightness(uint8_t brightness) {
  uint8_t payload[1] = { brightness };
  xmos_write_bytes(GPO_SERVICER_RESID, GPO_SERVICER_RESID_LED_BRIGHTNESS, payload, 1);
  Serial.println("LED brightness set.");
}
