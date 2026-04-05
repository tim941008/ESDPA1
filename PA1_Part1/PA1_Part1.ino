#include <Wire.h>
#include "I2C_GPIO.h"

// MPU6050 address while AD0 = GND.
static const uint8_t MPU_ADDR        = 0x68;

// ----------------------------------------------------------------------------
// Global Variable
// ----------------------------------------------------------------------------
bool systemEnabled = false;
bool scanExecuted  = false;


void address_scanner();
// ----------------------------------------------------------------------------
// Arduino Initialization
// ----------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("Part 1 ready. Send 'start' or 'stop'.");
}

// ----------------------------------------------------------------------------
// Arduino main loop
// ----------------------------------------------------------------------------
void loop() {
  // -- Processing Serial Port Command --
  // Enter "start" to wake up the MPU6050 and test once.
  // Enter "stop" to stop.
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();

    if (cmd.equalsIgnoreCase("start")) {
      systemEnabled = true;
      scanExecuted = false;

      Serial.println("System -> START");

      delay(100);
    }
    else if (cmd.equalsIgnoreCase("stop")) {
      systemEnabled = false;
      scanExecuted = false;

      Serial.println("System -> STOP");
    }
  }

  // Do nothing if system disabled
  if (!systemEnabled) return;

  // Run address scanner once after start
  if (!scanExecuted) {
    address_scanner();
    scanExecuted = true;
  }
}

// Scan all 7-bit I2C addresses from 0x00 to 0x7F and report which addresses ACK.
// Requirements:
// 1) Must use your GPIO-based I2C functions (NO Wire.h).
// 2) For each address, send a minimal probe transaction:
//    - START
//    - SLA+W (address + write bit)
//    - check ACK
//    - STOP
// 3) Print results via UART in BOTH decimal and hex.
// 4) If only one slave device is connected, exactly one address should ACK.
//
// Output format requirement (example):
//   "Addr: 104 (0x68) ACK"
//   "Scan done. Found: 1 device(s)"
void address_scanner()
{
  // Your implemteation
  int deviceCount = 0; // 用來記錄找到了幾個設備
  // 掃描所有 7-bit 位址 
  for (uint8_t addr = 0; addr < 128; addr++) {
    I2C_start();
    uint8_t sla_w = (addr << 1); //LSB 是 R/W bit，寫入時為 0
    //發送位址並檢查是否有收到 ACK
    bool has_ack = I2C_write_byte(sla_w);

    I2C_stop();

    if (has_ack) {
      Serial.print("Addr: ");
      Serial.print(addr, DEC);      // 印出十進制
      Serial.print(" (0x");
      if (addr < 16) Serial.print("0"); 
      Serial.print(addr, HEX);      // 印出十六進制
      Serial.println(") ACK");
      deviceCount++; 
    }
  }

  Serial.print("\nScan done. Found: ");
  Serial.print(deviceCount);
  Serial.println(" device(s)");
}

