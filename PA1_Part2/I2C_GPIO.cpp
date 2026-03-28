#include "I2C_GPIO.h"

// ----------------------------------------------------------------------------
// GPIO Tool APIs for Software I2C
// ----------------------------------------------------------------------------

void SDA_Low()
{
  // TODO: Configure SDA as output and drive it LOW.
}

// Release SDA (High-Z). SDA will be pulled HIGH by pull-up resistor.
void SDA_Release()
{
  // TODO: Configure SDA as input (high-impedance).
}

// Drive SCL LOW
void SCL_Low()
{
  // TODO: Configure SCL as output and drive it LOW.
}

// Release SCL (High-Z). SCL will be pulled HIGH by pull-up resistor.
void SCL_Release()
{
  // TODO: Configure SCL as input (high-impedance).
}

// Read SDA logic level
bool SDA_Read()
{
  // TODO: Return true if SDA is HIGH, false if SDA is LOW.
  // NOTE: ACK = LOW (0), NACK = HIGH (1)
}

// Read SCL logic level (optional, but useful for debug / clock stretching)
bool SCL_Read()
{
  // TODO: Return true if SCL is HIGH, false if SCL is LOW.
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
}

// Generate I2C REPEATED START condition
void I2C_repeated_start()
{
  // TODO:
  // Repeated START is the same electrical condition as START,
  // but the timing of use is different.
}

// Generate I2C STOP condition
void I2C_stop()
{
  // TODO:
  // STOP condition: SDA goes HIGH while SCL is HIGH.
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
}
