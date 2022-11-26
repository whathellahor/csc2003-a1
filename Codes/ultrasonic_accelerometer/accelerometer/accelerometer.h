#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#include <math.h>
#include "hardware/uart.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

// By default these devices are on bus address 0x68
static int addr = 0x68;

// absolute_time_t startTime;
// uint startTime;
// not using the default i2c pins. defined values are the selected pins for this project
#define SDA_PIN 16
#define SCL_PIN 17

//static absolute_time_t startTime;
extern float accUp;

// set flag and stable values for when car first power up to store initialized values
extern bool startFlag;        // car not start yet.
extern float stableAcc, min, max; // store value for when car first power up
// boolean to check for presence of hump. default value is 0 for no hump detected
extern bool hump;
// check if car is at the peak of the hump
extern bool peak;

#ifdef i2c_default
static void mpu6050_reset();
static void mpu6050_read_raw(int16_t accel[3], int16_t gyro[3], int16_t *temp);
#endif

void hump_calculation(float Acc);
extern bool triggerAcc_callback();
void initAcc();

// int main()
// {
//     stdio_init_all();
// #if !defined(i2c_default) || !defined(SDA_PIN) || !defined(SCL_PIN)
// #warning i2c/mpu6050_i2c example requires a board with I2C pins
//     puts("Default I2C pins were not defined");
// #else

//     // Set pin that I2C0 will use. SDA and SCL pins 16 and 17 respectively
//     i2c_init(i2c_default, 400 * 1000);
//     // Set selected SDA and SCL pins to use the I2C function on the pico
//     gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
//     gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
//     // set SDA SCL pins as pull up resistor
//     gpio_pull_up(SDA_PIN);
//     gpio_pull_up(SCL_PIN);

//     // Make the I2C pins available to picotool
//     bi_decl(bi_2pins_with_func(SDA_PIN, SCL_PIN, GPIO_FUNC_I2C));

//     mpu6050_reset();

//     // call timer to iterate the function to check for the hump
//     repeating_timer_t triggerTimer;
//     add_repeating_timer_ms(250, triggerAcc_callback, NULL, &triggerTimer);

//     while (1)
//     {
//         tight_loop_contents();
//     }

// #endif
//     return 0;
// }

#endif /* ACCELEROMETER_H */
