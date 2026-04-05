#include "I2C_GPIO.h"

static const uint8_t MPU_ADDR = 0x68;

#define PWR_MGMT_1 0x6B
#define ACCEL_XOUT_H 0x3B

#define LED_PIN 6
#define POT_PIN A0
#define BTN_PIN 2

// Fixed. DO NOT CHANGE THE INTERVAL
#define ADC_SAMPLE_INTERVAL_MS 50
#define IMU_SAMPLE_INTERVAL_MS 200
// Fixed. DO NOT CHANGE THE INTERVAL

// ----------------------------------------------------------------------------
// Global Variable
// ----------------------------------------------------------------------------

/*******************************************************************************/
// Design your own FSM state using an enum type.
// The system must support three operating modes:
// 1. MODE_OFF      : LED remains OFF.
// 2. MODE_ADC_PWM  : LED brightness is controlled by ADC input (potentiometer).
// 3. MODE_IMU_PWM  : LED brightness is controlled by IMU accelerometer readings.

// Your implementation

enum FSMState
{
  MODE_OFF,
  MODE_ADC_PWM,
  MODE_IMU_PWM
};
FSMState currentState = MODE_OFF; // 系統初始狀態

// --- 時間追蹤變數  ---
unsigned long lastAdcTime = 0;
unsigned long lastImuTime = 0;

// --- 按鈕去彈跳變數 ---

unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 200;


/*******************************************************************************/
bool systemEnabled = false;

void MPU6050_wakeup();
bool readAccel_raw(int16_t *ax, int16_t *ay, int16_t *az);
float getTilt(int16_t ax, int16_t ay, int16_t az);

/*******************************************************************************/
// Implement the behavior functions for the three FSM states.
// Each function should define how the system behaves in that mode.

// Your implementation

void OffMode();
void AdcPwmMode();
void ImuPwmMode();
/*******************************************************************************/

// ----------------------------------------------------------------------------
// Arduino Initialization
// ----------------------------------------------------------------------------
void setup()
{
  Serial.begin(115200);
  while (!Serial)
    ;

  Serial.println("Part3 ready. Send 'start' or 'stop'.");
}

void loop()
{
  // -- Processing Serial Port Command --
  // Enter "start" to wake up the MPU6050 and test once.
  // Enter "stop" to stop.
  if (Serial.available())
  {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();

    if (cmd.equalsIgnoreCase("start"))
    {
      systemEnabled = true;
      /*******************************************************************************/
      // Start initial
      // Initialize the FSM state and LED status.
      // The system should start from MODE_OFF and ensure the LED is turned off.

      // Your implementation
      currentState = MODE_OFF; // 預設進入 OFF 模式
      pinMode(LED_PIN, OUTPUT);
      analogWrite(LED_PIN, 0);        // 確保 LED 是關的
      pinMode(BTN_PIN, INPUT_PULLUP); // 設定按鈕為上拉輸入 (假設按鈕按下是 LOW)
                                      
      /*******************************************************************************/

      MPU6050_wakeup();
      delay(100);

      Serial.println("FSM ready.");
      Serial.println("Press the button to switch modes: OFF -> ADC_PWM -> IMU_PWM");

      Serial.println("System -> START");
      Serial.println("Mode -> OFF");
    }
    else if (cmd.equalsIgnoreCase("stop"))
    {
      systemEnabled = false;
      /*******************************************************************************/
      // Stop initial
      // Reset the FSM state and LED status.
      // When the system stops, the FSM should return to MODE_OFF
      // and the LED should be turned off.

      // Your implementation
      currentState = MODE_OFF;
      analogWrite(LED_PIN, 0); // 確保系統停止時 LED 關閉
                               /*******************************************************************************/
      Serial.println("System -> STOP");
    }
  }

  // If the system is stopped, ignore button input and do nothing else
  if (!systemEnabled)
  {
    return;
  }

  /*******************************************************************************/
  // Use the button to switch FSM modes.
  // Each button press should move the FSM to the next mode in sequence:
  // MODE_OFF → MODE_ADC_PWM → MODE_IMU_PWM → MODE_OFF ...
  // You may implement button debounce if necessary.

  // Your implementation

  if ((millis() - lastDebounceTime) > debounceDelay && digitalRead(BTN_PIN) != HIGH)
  {
    lastDebounceTime = millis(); 
    
    switch (currentState)
    {
    case MODE_OFF:
      currentState = MODE_ADC_PWM;
      Serial.println("Mode -> ADC_PWM");
      break;

    case MODE_ADC_PWM:
      currentState = MODE_IMU_PWM;
      Serial.println("Mode -> IMU_PWM");
      break;

    case MODE_IMU_PWM:
      currentState = MODE_OFF;
      Serial.println("Mode -> OFF");
      break;
    }
  }

  /*******************************************************************************/

  /*******************************************************************************/
  // Implement the actual FSM behavior here.
  // Depending on the current mode, call the corresponding function:
  //   MODE_OFF      -> OffMode()
  //   MODE_ADC_PWM  -> AdcPwmMode()
  //   MODE_IMU_PWM  -> ImuPwmMode()

  // Your implementation
  switch (currentState)
  {
  case MODE_OFF:
    OffMode();
    break;
  case MODE_ADC_PWM:
    AdcPwmMode();
    break;
  case MODE_IMU_PWM:
    ImuPwmMode();
    break;
  }
  /*******************************************************************************/

} // main loop end

/*******************************************************************************/
// MODE_OFF
// Description:
// The LED should remain completely OFF.
// No sensing or computation is required in this mode.

// Your implementation
void OffMode()
{
  digitalWrite(LED_PIN, LOW); 
}
/*******************************************************************************/

/*******************************************************************************/
// MODE_ADC_PWM
// Description:
// The LED brightness is controlled by the potentiometer.
// The system should:
// 1. Read the analog value from POT_PIN using ADC.
// 2. Map the ADC value (0–1023) to a PWM duty cycle (0–255).
// 3. Update the LED brightness accordingly.
//
// The sampling rate must follow the fixed interval defined by
// ADC_SAMPLE_INTERVAL_MS.

// Your implementation
void AdcPwmMode()
{
  
  if (millis() - lastAdcTime >= ADC_SAMPLE_INTERVAL_MS)
  {
    lastAdcTime = millis();

    int adcValue = analogRead(POT_PIN);       
    int duty = map(adcValue, 0, 1023, 0, 255); 

    analogWrite(LED_PIN, duty); 

    
    Serial.print("Mode: ADC_PWM | ADC: ");
    Serial.print(adcValue);
    Serial.print(" | PWM: ");
    Serial.println(duty);
  }
}
/*******************************************************************************/

/*******************************************************************************/
// MODE_IMU_PWM
// Description:
// The LED brightness is controlled by the accelerometer
// readings from the MPU6050.
//
// The system should:
// 1. Read accelerometer data using the GPIO-based I2C interface.
// 2. Compute the tilt magnitude from the raw accelerometer values.
// 3. Map the tilt magnitude to a PWM duty cycle.
// 4. Adjust the LED brightness accordingly.
//
// The sampling rate must follow the fixed interval defined by
// IMU_SAMPLE_INTERVAL_MS.
void ImuPwmMode()
{

  int16_t ax, ay, az;
  float tilt = 0.0f;
  int duty = 0;
  /*****************************************/
  // Periodically read raw accelerometer data and compute tilt.
  //
  // Steps:
  // 1. Call readAccel_raw() to get ax, ay, az.
  // 2. Call getTilt(ax, ay, az) to compute the tilt value.

  // Your implementation
  if (millis() - lastImuTime >= IMU_SAMPLE_INTERVAL_MS)
  {
    lastImuTime = millis();

    if (readAccel_raw(&ax, &ay, &az))
    {
      tilt = getTilt(ax, ay, az);
    }
    else
    {
      Serial.println("IMU Read Error!");
      return; 
    }
    /*****************************************/

    /*****************************************/
    // Map the tilt magnitude to LED brightness.
    // Requirements:
    // 1. Convert tilt (0.0 ~ 1.0) to PWM duty cycle.
    // 2. Make sure the duty cycle stays within the valid PWM range.
    // 3. Output the duty cycle to the LED to control the brightness.

    // Your implementation

    // 將 tilt (0.0 ~ 1.0) 映射到 PWM (0 ~ 255)
    duty = (int)(tilt * 255.0f);

    // 確保不會超過範圍
    if (duty > 255)
      duty = 255;
    if (duty < 0)
      duty = 0;

    analogWrite(LED_PIN, duty);
  }
  else
  {
    return; // 時間還沒到，直接結束函式，不要印出東西
  }
  /*****************************************/

  Serial.print(" | PWM: ");
  Serial.println(duty);
}
/*******************************************************************************/

/*******************************************************************************/
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
  I2C_write_byte((MPU_ADDR << 1) | 0); 
  I2C_write_byte(PWR_MGMT_1);          
  I2C_write_byte(0x00);                
  I2C_stop();
}
/*******************************************************************************/

/*******************************************************************************/
// Key part: Read raw accelerometer data from MPU6050.
//
// This function performs a complete I2C register read transaction using
// the GPIO-based I2C implementation.
//
// Requirements:
// 1. Use a register pointer write to select the starting register.
// 2. Use a repeated START condition to switch to read mode.
// 3. Read six bytes of data (XH, XL, YH, YL, ZH, ZL).
// 4. Combine high and low bytes into 16-bit signed integers.
//
// Transaction flow:
//
//   START
//   SLA+W
//   register address (ACCEL_XOUT_H)
//   REPEATED START
//   SLA+R
//   read XH (ACK)
//   read XL (ACK)
//   read YH (ACK)
//   read YL (ACK)
//   read ZH (ACK)
//   read ZL (NACK)
//   STOP
//
// The function returns true if the transaction succeeds.
bool readAccel_raw(int16_t *ax, int16_t *ay, int16_t *az)
{

  // Your implementation
  //
  // Requirements:
  // 1. Read 6 consecutive bytes starting from ACCEL_XOUT_H.
  // 2. Use a repeated START to switch from write mode to read mode.
  // 3. Store the combined 16-bit signed results into *ax, *ay, and *az.
  // 4. Return true if the whole transaction succeeds; otherwise return false.
  
  I2C_start();
  if (!I2C_write_byte((MPU_ADDR << 1) | 0))
    return false;
  if (!I2C_write_byte(ACCEL_XOUT_H))
    return false;

  I2C_repeated_start(); // 改變資料方向

  if (!I2C_write_byte((MPU_ADDR << 1) | 1))
    return false;
  uint8_t xh = I2C_read_byte(true);
  uint8_t xl = I2C_read_byte(true);
  uint8_t yh = I2C_read_byte(true);
  uint8_t yl = I2C_read_byte(true);
  uint8_t zh = I2C_read_byte(true);
  uint8_t zl = I2C_read_byte(false);// 最後一個不用ack，讀夠了
  I2C_stop();

  // 合併
  *ax = (int16_t)((xh << 8) | xl);
  *ay = (int16_t)((yh << 8) | yl);
  *az = (int16_t)((zh << 8) | zl);
  return true;
}
/*******************************************************************************/

/*****************************************************************************/
// Convert raw accelerometer values to tilt magnitude.
//
// Parameters:
//   ax, ay, az : raw 16-bit accelerometer values from MPU6050
//
// Return:
//   tilt magnitude in the range [0.0, 1.0]
//
// Steps:
//   1. Convert raw values to acceleration in g
//   2. Compute tilt magnitude from X and Y components
//   3. Clamp the result to [0,1]
/*****************************************************************************/
float getTilt(int16_t ax, int16_t ay, int16_t az)
{
  float gx = ax / 16384.0f;
  float gy = ay / 16384.0f;
  float gz = az / 16384.0f;

  Serial.print("Mode: IMU_PWM | Acc[g]: ");
  Serial.print(gx, 3);
  Serial.print(", ");
  Serial.print(gy, 3);
  Serial.print(", ");
  Serial.print(gz, 3);
  float tilt = sqrt(gx * gx + gy * gy);

  if (tilt > 1.0f)
    tilt = 1.0f;

  return tilt;
}
