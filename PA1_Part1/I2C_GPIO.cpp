#include "I2C_GPIO.h"

// ----------------------------------------------------------------------------
// GPIO Tool APIs for Software I2C
// ----------------------------------------------------------------------------

void SDA_Low()
{
  // TODO: Configure SDA as output and drive it LOW.
  pinMode(I2C_SDA, OUTPUT);
  digitalWrite(I2C_SDA, LOW);
}

// Release SDA (High-Z). SDA will be pulled HIGH by pull-up resistor.
void SDA_Release()
{
  // TODO: Configure SDA as input (high-impedance).
  pinMode(I2C_SDA, INPUT_PULLUP);
}

// Drive SCL LOW
void SCL_Low()
{
  // TODO: Configure SCL as output and drive it LOW.
  pinMode(I2C_SCL, OUTPUT);
  digitalWrite(I2C_SCL, LOW);
}

// Release SCL (High-Z). SCL will be pulled HIGH by pull-up resistor.
void SCL_Release()
{
  // TODO: Configure SCL as input (high-impedance).
  pinMode(I2C_SCL, INPUT_PULLUP);
}

// Read SDA logic level
bool SDA_Read()
{
  // TODO: Return true if SDA is HIGH, false if SDA is LOW.
  // NOTE: ACK = LOW (0), NACK = HIGH (1)
  pinMode(I2C_SDA, INPUT_PULLUP);
  return digitalRead(I2C_SDA);
}

// Read SCL logic level (optional, but useful for debug / clock stretching)
bool SCL_Read()
{
  // TODO: Return true if SCL is HIGH, false if SCL is LOW.
  pinMode(I2C_SCL, INPUT_PULLUP);
  return digitalRead(I2C_SCL);
}

// Timing delay
void delay_t()
{
  delayMicroseconds(10);
}



// ----------------------------------------------------------------------------
// Software I2C Primitive Operations

// ----------------------------------------------------------------------------

// Generate I2C START condition
void I2C_start()
{
  // TODO:
  // START condition: SDA goes LOW while SCL is HIGH, then pull SCL LOW to enter data phase.
  SDA_Release();
  SCL_Release();
  delay_t();

  SDA_Low();
  delay_t();

  SCL_Low();
  delay_t();
}

// Generate I2C REPEATED START condition
void I2C_repeated_start()
{
  // TODO:
  // Repeated START is the same electrical condition as START,
  // but the timing of use is different.

  // 1. 在 SCL 為 LOW 時，先將 SDA 釋放 (拉高)
  SDA_Release();
  delay_t();

  // 2. 將 SCL 釋放 (拉高)
  SCL_Release();
  delay_t();

  // 3. 執行標準的 START 條件：在 SCL 為 HIGH 時拉低 SDA
  SDA_Low();
  delay_t();

  // 4. 將 SCL 拉低，繼續傳輸
  SCL_Low();
  delay_t();
}

// Generate I2C STOP condition
void I2C_stop()
{
  // TODO:
  // STOP condition: SDA goes HIGH while SCL is HIGH.
  SDA_Low();
  delay_t();

  SCL_Release();
  delay_t();

  SDA_Release();
  delay_t();

}

// Read a single bit from SDA during SCL high phase
bool I2C_read_bit()
{
  // TODO:
  // 1) Release SDA (slave drives SDA)
  // 2) Raise SCL (release SCL)
  // 3) Sample SDA while SCL is HIGH
  // 4) Pull SCL LOW
  // 5) Return sampled bit

  // 1) 釋放 SDA，讓 Slave (從機) 可以控制這條線
  SDA_Release();
  delay_t();

  // 2) 將 SCL 拉高，進入讀取週期
  SCL_Release();
  delay_t();

  // 3) 在 SCL 為 HIGH 時，讀取 SDA 的狀態
  bool bit_value = SDA_Read();

  // 4) 將 SCL 拉低，結束這個位元的週期
  SCL_Low();
  delay_t();

  // 5) 回傳讀到的值 (HIGH 為 true, LOW 為 false)
  return bit_value;
}

// Write one byte (MSB-first) and read ACK/NACK from slave
// Return true if ACK received (ACK=0), false if NACK (ACK=1)
bool I2C_write_byte(uint8_t data)
{
  // TODO:
  // 1) Send 8 data bits (MSB-first) on SDA, toggling SCL for each bit
  // 2) Release SDA for ACK phase
  // 3) Read ACK bit from slave on 9th clock
  // 4) Return true if ACK (LOW), false if NACK (HIGH)

  for (int i = 0; i < 8; i++) {
    // 檢查目前資料的最高位元是 1 還是 0
    if (data & 0x80) {
      SDA_Release(); // 如果是 1，釋放 SDA 讓它變 HIGH
    } else {
      SDA_Low();     // 如果是 0，拉低 SDA
    }
    delay_t();

    // 敲擊一次時脈 (Clock pulse)：拉高再拉低
    SCL_Release(); // 讓 Slave 讀取這個位元
    delay_t();
    SCL_Low();     // 準備送下一個位元
    delay_t();

    // 將資料往左推一位元，準備送下一個 bit
    data = data << 1; 
  }

  // 2 & 3) 釋放 SDA 並讀取第 9 個時鐘的 ACK 位元
  // 這裡直接呼叫我們剛剛寫好的 I2C_read_bit()
  bool ack_bit = I2C_read_bit(); 

  // 4) 根據規定：ACK 是 LOW (0)，NACK 是 HIGH (1)
  // 我們的函式要求收到 ACK 要回傳 true，所以要反轉它
  return !ack_bit;


}

// Read one byte (MSB-first) and then drive ACK/NACK on 9th clock
// ack=true  -> master sends ACK (LOW) to request more bytes
// ack=false -> master sends NACK (HIGH) to end reading
uint8_t I2C_read_byte(bool ack)
{
  // TODO:
  // 1) For 8 bits: read bit-by-bit while toggling SCL
  // 2) On 9th clock: master drives ACK (LOW) or releases SDA for NACK
  // 3) Return the byte
  uint8_t data = 0;

  // 1) 接收 8 個資料位元 (MSB-first)
  for (int i = 0; i < 8; i++) {
    // 把目前的資料往左推一格，騰出最右邊的位置給新來的 bit
    data = data << 1; 

    // 呼叫剛剛寫好的函式讀取一個 bit
    if (I2C_read_bit()) {
      // 如果讀到的是 HIGH (1)，就把 data 的最右邊那個 bit 變成 1
      data = data | 0x01; 
    }
    // 如果讀到的是 LOW (0)，因為往左推的時候右邊本來就會補 0，所以不用特別處理
  }

  // 2) 第 9 個時鐘週期：主機發送 ACK 或 NACK 給從機
  if (ack) {
    SDA_Low();     // 準備發送 ACK (LOW)
  } else {
    SDA_Release(); // 準備發送 NACK (HIGH，釋放總線)
  }
  delay_t();

  // 敲擊第 9 次時脈 (Clock pulse)，把 ACK/NACK 訊號送出去
  SCL_Release();
  delay_t();
  SCL_Low();
  delay_t();

  // 3) 回傳讀到的完整 8-bit 資料
  return data;

}
