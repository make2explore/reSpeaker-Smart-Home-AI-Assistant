#include <Wire.h>

// Define the 7-bit I2C address of the XVF3800 device
#define XMOS_ADDR 0x2C  

// Define XVF3800 Resource and Command IDs
#define GPO_SERVICER_RESID 20                         // Resource ID for GPIO Output (GPO)
#define GPO_SERVICER_RESID_GPO_WRITE_VALUE 1          // Command ID to write value to GPIO
#define IO_CONFIG_SERVICER_RESID 36                   // Resource ID for IO Configuration
#define IO_CONFIG_SERVICER_RESID_GPI_VALUE_ALL 6      // Command ID to read all GPIO input values

void setup() {
  Wire.begin();                 // Initialize I2C communication
  Serial.begin(115200);        // Initialize serial communication for debugging
  delay(1000);                 // Short delay to allow device startup

  Serial.println("Muting Mic (Setting GPIO 30 HIGH)");
  muteMic();                   // Set GPIO 30 HIGH to mute microphone
  delay(5000);                 // Wait for 5 seconds

  Serial.println("Unmuting Mic (Setting GPIO 30 LOW)");
  unmuteMic();                 // Set GPIO 30 LOW to unmute microphone
  delay(3000);                 // Wait for 3 seconds

  Serial.println("Reading GPIO Status...");
  readGPIOStatus();            // Read and print the status of all GPIOs
}

void loop() {
  // Empty loop - no repeated actions for now
}

// Function to set GPIO 30 to a specific logic level (0 = LOW, 1 = HIGH)
void setGPIO30(uint8_t level) {
  uint8_t payload[2] = {30, level};  // Payload format: [GPIO index, value]
  xmos_write_bytes(GPO_SERVICER_RESID, GPO_SERVICER_RESID_GPO_WRITE_VALUE, payload, 2);
  
  Serial.print("Command Sent: GPIO 30 = ");
  Serial.println(level);
}

// Convenience function to mute the microphone (set GPIO 30 HIGH)
void muteMic() {
  setGPIO30(1);  // Logic HIGH to mute
}

// Convenience function to unmute the microphone (set GPIO 30 LOW)
void unmuteMic() {
  setGPIO30(0);  // Logic LOW to unmute
}

// Function to write a sequence of bytes over I2C to the XVF3800
void xmos_write_bytes(uint8_t resid, uint8_t cmd, uint8_t *value, uint8_t write_byte_num) {
  Wire.beginTransmission(XMOS_ADDR); // Begin I2C transmission to XVF3800
  Wire.write(resid);                 // Write the resource ID
  Wire.write(cmd);                   // Write the command ID
  Wire.write(write_byte_num);       // Write number of payload bytes
  for (uint8_t i = 0; i < write_byte_num; i++) {
    Wire.write(value[i]);           // Write each payload byte
  }
  Wire.endTransmission();           // End the I2C transmission
}

// Function to read the status of all GPIO inputs (32 bits) from XVF3800
void readGPIOStatus() {
  uint8_t buffer[4] = {0};  // Buffer to hold the 4-byte GPIO status response

  // --- Write phase: Send read request ---
  Wire.beginTransmission(XMOS_ADDR);                        // Begin I2C write transaction
  Wire.write(IO_CONFIG_SERVICER_RESID);                     // Write the resource ID for IO config
  Wire.write(IO_CONFIG_SERVICER_RESID_GPI_VALUE_ALL);       // Write the command ID to get all GPIO values
  Wire.write(1);                                             // Payload length (1 byte)
  Wire.endTransmission(false);                              // End transmission with repeated start (no stop)

  // --- Read phase: Read response from device ---
  Wire.requestFrom(XMOS_ADDR, 5); // Request 5 bytes: 1 status byte + 4 data bytes

  if (Wire.available() < 5) {
    Serial.println("Error: Not enough bytes received from XVF3800.");
    return;
  }

  uint8_t status = Wire.read();  // Read the status byte (should be 0 for success)

  // Read the 4-byte GPIO input status value
  for (int i = 0; i < 4; i++) {
    buffer[i] = Wire.read();
  }

  // Combine 4 bytes into a single 32-bit unsigned integer
  uint32_t gpio_status = ((uint32_t)buffer[3] << 24) |
                         ((uint32_t)buffer[2] << 16) |
                         ((uint32_t)buffer[1] << 8)  |
                         ((uint32_t)buffer[0]);

  Serial.print("GPIO Status Register = 0x");
  Serial.println(gpio_status, HEX);

  // Check and print the state of GPIO 30 specifically
  bool gpio30 = (gpio_status >> 30) & 0x01;
  Serial.print("GPIO 30 State: ");
  Serial.println(gpio30 ? "HIGH (Muted)" : "LOW (Unmuted)");
}
