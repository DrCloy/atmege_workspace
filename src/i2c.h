/**
 * @file i2c.h
 * @brief Header file for I2C communication functions.
 * 
 * This file contains macros and inline functions for I2C communication
 * using the TWI (Two-Wire Interface) module in ATmega128.
 */

#ifndef I2C_H
#define I2C_H

#include <avr/io.h>
#include <avr/iom128.h>

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#define SCL_CLOCK 100000UL // 100kHz I2C frequency

// Check whether action of twi has been completed
#define I2C_IS_ACTION_COMPLETE (TWCR & (1 << TWINT))

// Send start condition
#define I2C_ACTION_START (TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN))

// Send stop condition
#define I2C_ACTION_STOP (TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN))

// Write data to twi
#define I2C_ACTION_WRITE(data) (TWDR = data, TWCR = (1 << TWINT) | (1 << TWEN))

// Read data from twi
#define I2C_ACTION_READ(ack) (TWCR = (1 << TWINT) | (1 << TWEN) | (ack << TWEA))

static inline void i2c_init() {
    TWSR = 0; // No prescaler
    TWBR = ((F_CPU / SCL_CLOCK) - 16) / 2; // Set SCL frequency
}

/**
 * @brief Send a start condition to TWI(I2C) and wait until the action is complete.
 */
static inline void i2c_start() {
    I2C_ACTION_START;
    while (!I2C_IS_ACTION_COMPLETE);
}

/**
 * @brief Send a stop condition to TWI(I2C).
 */
static inline void i2c_stop() {
    I2C_ACTION_STOP;
}

/**
 * @brief Write data to TWI(I2C).
 * @param data The data to be written to the TWI bus.
 */
static inline void i2c_write(uint8_t data) {
    I2C_ACTION_WRITE(data);
    while (!I2C_IS_ACTION_COMPLETE);
}

/**
 * @brief Read data from TWI(I2C).
 * 
 * @param ack The Acknowledge signal: 1 is ACK, 0 is NACK.
 * @return The data read from the TWI bus.
 */
static inline uint8_t i2c_read(int ack) {
    I2C_ACTION_READ(ack);
    while (!I2C_IS_ACTION_COMPLETE);
    return TWDR; // Read data from TWDR register
}

#endif // I2C_H