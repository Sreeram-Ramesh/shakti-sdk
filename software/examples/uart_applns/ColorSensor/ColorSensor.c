#include <stdint.h>
#include "i2c.h"
#include "log.h"
#include "uart.h"

// Delay definitions
#define DELAY1          100
#define DELAY2          50
#define DELAY_VALUE     900

// I2C instance configuration
#define PRESCALER_COUNT 24
#define SCLK_COUNT      9
#define I2C             i2c_instance[1]

// APDS9960 I2C address (7-bit 0x39 shifted left by one for write/read)
#define APDS9960_SLAVE_ADDRESS  0x72

// APDS9960 register definitions
#define APDS9960_ENABLE   0x80    // Enable register: bits for power on (PON) and ALS enable (AEN)
#define APDS9960_ATIME    0x82    // ALS integration time register
#define APDS9960_CONTROL  0x8F    // Gain control register
#define APDS9960_ID       0x92    // Device ID register

// Color data registers
#define APDS9960_CDATAL   0x94    // Clear data low byte
#define APDS9960_CDATAH   0x95    // Clear data high byte
#define APDS9960_RDATAL   0x96    // Red data low byte
#define APDS9960_RDATAH   0x97    // Red data high byte
#define APDS9960_GDATAL   0x98    // Green data low byte
#define APDS9960_GDATAH   0x99    // Green data high byte
#define APDS9960_BDATAL   0x9A    // Blue data low byte
#define APDS9960_BDATAH   0x9B    // Blue data high byte

// Function prototypes
int  read_apds9960_register(i2c_struct *I2C, unsigned int reg_offset,
                            unsigned char *read_data, unsigned char length,
                            unsigned long delay);
int  write_apds9960_register(i2c_struct *I2C, unsigned int reg_offset,
                             unsigned int *write_value, unsigned long delay);
unsigned char CheckAPDS9960DeviceId(i2c_struct *I2C, unsigned long delay);
void apds9960_init(i2c_struct *I2C, unsigned long delay);
void read_color_data(i2c_struct *I2C, unsigned long delay,
                     uint16_t *clear, uint16_t *red, uint16_t *green, uint16_t *blue);

//
// Read a single register from the APDS9960 (similar to the MPU6050 routine)
//
int read_apds9960_register(i2c_struct *I2C, unsigned int reg_offset,
                           unsigned char *read_data, unsigned char length,
                           unsigned long delay)
{
    unsigned char temp = 0;
    
    // Write: Send slave address with write flag
    i2c_send_slave_address(I2C, APDS9960_SLAVE_ADDRESS, I2C_WRITE, delay);
    // Write the register offset to be read
    i2c_write_data(I2C, reg_offset, delay);
    
    // Repeated start to begin reading
    I2C->control = I2C_REPSTART;
    delay_loop(DELAY1, DELAY2);
    
    // Write: Send slave address with read flag
    i2c_write_data(I2C, APDS9960_SLAVE_ADDRESS | I2C_READ, delay);
    
    // Dummy read as required by the I2C controller
    I2C->control = I2C_NACK;
    i2c_read_data(I2C, &temp, delay);
    
    // Read actual data
    i2c_read_data(I2C, &temp, delay);
    *read_data = temp;
    
    // Send stop condition
    I2C->control = I2C_STOP;
    delay_loop(DELAY1, DELAY2);
    return 0;
}

//
// Write a single register to the APDS9960 (following the MPU6050 driver style)
//
int write_apds9960_register(i2c_struct *I2C, unsigned int reg_offset,
                            unsigned int *write_value, unsigned long delay)
{
    I2C->control = I2C_IDLE;
    delay_loop(DELAY1, DELAY2);
    
    // Send slave address for write
    i2c_send_slave_address(I2C, APDS9960_SLAVE_ADDRESS, I2C_WRITE, delay);
    // Write the register offset
    i2c_write_data(I2C, reg_offset, delay);
    log_debug("Writing value: %x to register: %x", *write_value, reg_offset);
    // Write the data byte
    i2c_write_data(I2C, (*write_value & 0xff), delay);
    
    // Send stop condition
    I2C->control = I2C_STOP;
    delay_loop(DELAY1, DELAY2);
    
    return 0;
}

//
// Check APDS9960 device ID. Expected ID is assumed to be 0xAB (adjust as needed).
//
unsigned char CheckAPDS9960DeviceId(i2c_struct *I2C, unsigned long delay)
{
    unsigned char id = 0xFF;
    read_apds9960_register(I2C, APDS9960_ID, &id, 1, delay);
    log_debug("APDS9960 Device ID Read: %x", id);
    return id;
}

//
// Initialize the APDS9960 as a color sensor
//
void apds9960_init(i2c_struct *I2C, unsigned long delay)
{
    unsigned int write_val = 0;
    
    // Check the device ID
    unsigned char device_id = CheckAPDS9960DeviceId(I2C, delay);
    if(device_id != 0xAB) {
        log_error("APDS9960 Device ID mismatch. Expected 0xAB, got %x", device_id);
    } else {
        log_info("APDS9960 Device ID verified.");
    }
    
    // Enable the device:
    // Write 0x03 to the ENABLE register to set PON (Power ON) and AEN (ALS Enable)
    write_val = 0x03;
    write_apds9960_register(I2C, APDS9960_ENABLE, &write_val, delay);
    
    // Set integration time (ATIME). 0xFF gives a short integration period (≈2.4ms).
    write_val = 0xFF;
    write_apds9960_register(I2C, APDS9960_ATIME, &write_val, delay);
    
    // Set the gain in the CONTROL register.
    // For example, 0x01 selects a 4x gain.
    write_val = 0x01;
    write_apds9960_register(I2C, APDS9960_CONTROL, &write_val, delay);
    
    log_info("APDS9960 initialization complete.");
}

//
// Read color data (Clear, Red, Green, Blue) from the APDS9960.
//
void read_color_data(i2c_struct *I2C, unsigned long delay,
                     uint16_t *clear, uint16_t *red, uint16_t *green, uint16_t *blue)
{
    uint8_t low = 0, high = 0;
    
    // Read Clear channel (low and high bytes)
    read_apds9960_register(I2C, APDS9960_CDATAL, &low, 1, delay);
    read_apds9960_register(I2C, APDS9960_CDATAH, &high, 1, delay);
    *clear = (high << 8) | low;
    
    // Read Red channel
    read_apds9960_register(I2C, APDS9960_RDATAL, &low, 1, delay);
    read_apds9960_register(I2C, APDS9960_RDATAH, &high, 1, delay);
    *red = (high << 8) | low;
    
    // Read Green channel
    read_apds9960_register(I2C, APDS9960_GDATAL, &low, 1, delay);
    read_apds9960_register(I2C, APDS9960_GDATAH, &high, 1, delay);
    *green = (high << 8) | low;
    
    // Read Blue channel
    read_apds9960_register(I2C, APDS9960_BDATAL, &low, 1, delay);
    read_apds9960_register(I2C, APDS9960_BDATAH, &high, 1, delay);
    *blue = (high << 8) | low;
}

//
// Main function: Initialize I2C and the APDS9960, then continuously read and log color data.
//
int main()
{
    unsigned long delay = 500;
    uint16_t clear, red, green, blue;
    
    log_info("APDS9960 Color Sensor Driver - SHAKTI SOC IITM");
    
    // Initialize I2C interface
    i2c_init();
    if(config_i2c(I2C, PRESCALER_COUNT, SCLK_COUNT))
    {
        log_error("I2C Initialization Failed.");
        return -1;
    }
    else {
        log_info("I2C Initialization Successful.");
    }
    
    // Initialize the APDS9960 sensor
    apds9960_init(I2C, delay);
    
    log_info("Entering main loop to read color data.");
    while(1)
    {
        read_color_data(I2C, delay, &clear, &red, &green, &blue);
        log_info("Color Data - Clear: %d, Red: %d, Green: %d, Blue: %d", clear, red, green, blue);
        delay_loop(2000, 1000);
    }
    
    return 0;
}
