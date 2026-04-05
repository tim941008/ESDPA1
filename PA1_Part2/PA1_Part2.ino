#include "I2C_GPIO.h"

static const uint8_t MPU_ADDR = 0x68;

#define PWR_MGMT_1   0x6B
#define WHO_AM_I     0x75
// ----------------------------------------------------------------------------
// Global Variable
// ----------------------------------------------------------------------------

bool systemEnabled = false;
bool testExecuted = false;



void MPU6050_wakeup();
uint8_t READ_WHO_AM_I_REG();
uint8_t READ_PWR_MGMT_1_REG();
// ----------------------------------------------------------------------------
// Arduino Initialization
// ----------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  while (!Serial);         

  Serial.println("Part2 ready. Send 'start' or 'stop'.");
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
      testExecuted = false;
      Serial.println("System -> START");
/*******************************************************************************/
// First, call MPU6050_wakeup() to clear the sleep bit in PWR_MGMT_1.
// Then, call Read_PWR_MGMT_1_REG() to read back the register value.
// The returned value should be 0x00 if the MPU6050 has been successfully awakened.
      
  // Your implementation
  
  MPU6050_wakeup(); 
  uint8_t pwr_state = READ_PWR_MGMT_1_REG(); // 讀回狀態
      
  Serial.print("PWR_MGMT_1 = 0x");
  Serial.println(pwr_state, HEX);
      
  if (pwr_state == 0x00) {
    Serial.println("Success wake up!\n");
  }
/*******************************************************************************/
      delay(100);
    }
    else if (cmd.equalsIgnoreCase("stop")) {
      systemEnabled = false;
      testExecuted = false;

      Serial.println("System -> STOP");
    }
  }

  // Do nothing if system disabled
  if (!systemEnabled) return;

  
  if (!testExecuted) {
/*******************************************************************************/
// Read the WHO_AM_I register 5 times and print each result.
// For each read, if the returned value is 0x68, increase a counter by 1.
// After 5 reads, print the total count and check whether it equals 5.
// The output should match the expected result shown in the PA1 document.

  // Your implementation
  Serial.println("Read five times of WHO_AM_I register.");
  int Count = 0;
    
  for (int i = 0; i < 5; i++) {

    uint8_t reply = READ_WHO_AM_I_REG();
    Serial.print("reply = 0x");
    Serial.println(reply, HEX);
    
    if (reply == 0x68) {
      Count++;
    }
  }
    
  if (Count == 5) {
    Serial.println("\nSuccess!");
    Serial.println("Got 5 replies of MPU address.");
  }
/*******************************************************************************/

    testExecuted = true;
  }
}

// ----------------------------------------------------------------------------
// GPIO-based I2C: Wake up MPU6050
// ----------------------------------------------------------------------------

// Wake up MPU6050 by writing to its power management register.
// Requirements:
// 1) Must use your GPIO-based I2C functions (NO Wire.h).
// 2) Must perform a valid I2C register write transaction:
//    - START
//    - SLA+W
//    - register address
//    - data
//    - STOP
// Notes:
// - MPU6050 PWR_MGMT_1 register address: 0x6B
// - Wakeup value: 0x00 (clear sleep bit)
void MPU6050_wakeup()
{
  // Your implemteation
  I2C_start();
  I2C_write_byte((MPU_ADDR << 1) | 0); // 傳送 SLA+W 
  I2C_write_byte(PWR_MGMT_1);          // 指定 PWR_MGMT_1 暫存器 (0x6B)
  I2C_write_byte(0x00);                // 寫入 0x00 清除 Sleep bit
  I2C_stop();
}



// ----------------------------------------------------------------------------
// Read the PWR_MGMT_1 register from the MPU6050 sensor.
// ----------------------------------------------------------------------------
//
// Requirements:
// 1) Must use your GPIO-based I2C functions (NO Wire.h).
// 2) Perform the following I2C transaction sequence:
//    - START
//    - SLA+W (MPU address + write bit)
//    - wait for ACK
//    - send register address (0x6B)
//    - wait for ACK
//    - REPEATED START
//    - SLA+R (MPU address + read bit)
//    - wait for ACK
//    - read 1 byte from the device
//    - send ACK / NACK
//    - STOP
//
// Return value:
// // The value read from the PWR_MGMT_1 register.
uint8_t READ_PWR_MGMT_1_REG()
{
  // Your implementation
  I2C_start();
  I2C_write_byte((MPU_ADDR << 1) | 0); // 傳送 SLA+W
  I2C_write_byte(PWR_MGMT_1);            // 指定 PWR_MGMT_1 暫存器 
  
  I2C_repeated_start();                // 重複啟動，準備改變資料方向
  
  I2C_write_byte((MPU_ADDR << 1) | 1); // 傳送 SLA+R
  // 讀取 1 byte，傳入 false 代表主機要回傳 NACK (結束讀取)
  uint8_t val = I2C_read_byte(false);  
  I2C_stop();
  return val;
}


// ----------------------------------------------------------------------------
// Read the WHO_AM_I register from the MPU6050 sensor.
// ----------------------------------------------------------------------------

// Requirements:
// 1) Must use your GPIO-based I2C functions (NO Wire.h).
// 2) Perform the following I2C transaction sequence:
//    - START
//    - SLA+W (MPU address + write bit)
//    - wait for ACK
//    - send register address (0x75)
//    - wait for ACK
//    - REPEATED START
//    - SLA+R (MPU address + read bit)
//    - wait for ACK
//    - read 1 byte from the device
//    - send ACK / NACK
//    - STOP
//
// Return value:
// The value read from the WHO_AM_I register.

uint8_t READ_WHO_AM_I_REG()
{
   // Your implementation
  I2C_start();
  I2C_write_byte((MPU_ADDR << 1) | 0); // 傳送 SLA+W
  I2C_write_byte(WHO_AM_I);            // 指定 WHO_AM_I 暫存器 (0x75)
  
  I2C_repeated_start();                // 重複啟動，準備改變資料方向
  
  I2C_write_byte((MPU_ADDR << 1) | 1); // 傳送 SLA+R 
  
  // 讀取 1 byte，傳入 false 代表主機要回傳 NACK (結束讀取)
  uint8_t val = I2C_read_byte(false);  
  I2C_stop();
  return val;
}