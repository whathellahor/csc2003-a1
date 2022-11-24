/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

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

//not using the default i2c pins. defined values are the selected pins for this project
#define SDA_PIN 16
#define SCL_PIN 17

//boolean to check for presence of hump. default value is 0 for no hump detected
bool hump = 0;
static absolute_time_t startTime;
float accY;


// functions
void hump_calculation(float Ay, uint time);
bool triggerAcc_callback(struct repeating_timer *t);

#ifdef i2c_default
static void mpu6050_reset()
{
    // Two byte reset. First byte register, second byte data
    //0x6B is power management 1
    //Reset value is 0x00 for most registers (apart from 0x40, 0x68)
    uint8_t buf[] = {0x6B, 0x00};
    i2c_write_blocking(i2c_default, addr, buf, 2, false);
}

static void mpu6050_read_raw(int16_t accel[3], int16_t gyro[3], int16_t *temp)
{
    // For this particular device, we send the device the register we want to read
    // first, then subsequently read from the device. The register is auto incrementing
    // so we don't need to keep sending the register we want, just the first.

    uint8_t buffer[6];

    //3B is the register for accelerometer(High) output
    // Start reading acceleration registers from register 0x3B for 6 bytes 
    //(Registers  59 to 64)
    uint8_t val = 0x3B;
    //i2c_write blocking: attempt to write specified number of bytes to address. 
    i2c_write_blocking(i2c_default, addr, &val, 1, true); // true to keep master control of bus
    //i2c_read blocking: Attempt to read specified number of bytes from address, 
    i2c_read_blocking(i2c_default, addr, buffer, 6, false);

    for (int i = 0; i < 3; i++)
    {
        accel[i] = (buffer[i * 2] << 8 | buffer[(i * 2) + 1]);
    }

}
#endif

int main()
{
    stdio_init_all();
#if !defined(i2c_default) || !defined(SDA_PIN) || !defined(SCL_PIN)
#warning i2c/mpu6050_i2c example requires a board with I2C pins
    puts("Default I2C pins were not defined");
#else

    // Set pin that I2C0 will use. SDA and SCL pins 16 and 17 respectively
    i2c_init(i2c_default, 400 * 1000);
    //Set selected SDA and SCL pins to use the I2C function on the pico
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    //set SDA SCL pins as pull up resistor
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    // Make the I2C pins available to picotool
    bi_decl(bi_2pins_with_func(SDA_PIN, SCL_PIN, GPIO_FUNC_I2C));

    mpu6050_reset();

    //call timer to iterate the function to check for the hump
    repeating_timer_t triggerTimer;
    add_repeating_timer_ms(500, triggerAcc_callback, NULL, &triggerTimer);

    while (1)
    {
        tight_loop_contents();
    }

#endif
    return 0;
}

void hump_calculation(float Ay, uint time)
{
    float height = 0;

    height = Ay * pow(time, 2); //formula to calculate height of the hump
    printf("time going up the hump: %lld\n", time);
    printf("Height is: %.2f cm\n\n", height);
    // return height;
}

bool triggerAcc_callback(repeating_timer_t *t)
{
    // array variables to store accelerometer values
    int16_t acceleration[3], gyro[3], temp;
    //calling the method to obtain the raw values that the acclerometer read
    mpu6050_read_raw(acceleration, gyro, &temp);

    float Ax, Ay, Az;
    float Gx, Gy, Gz;

    // uint time;

    //+- 4g Full Scale Range. 8192 LSB Sensitivity
    Ax = acceleration[0] / 8192; 
    Ay = acceleration[1] / 8192;
    Az = acceleration[2] / 8192;

    //
    printf("Raw Acceleration is. X = %.2f, Y = %.2f, Z = %.2f\n", Ax, Ay, Az);
    printf("Raw Gyro is. X = %.2f, Y = %.2f, Z = %.2f\n", Gx, Gy, Gz);

    if (Ay > 0 && hump == 0)
    {   
        //to get the snapshot of the time the moment the car goes up the hump
        startTime = get_absolute_time();


        printf("Hump detected.\n");
        accY = Ay;  //assign the value of the acceleration at the X axis to the global value the moment the car goes up the hump
        hump = 1; // hump detected, set boolean to true
    }
    if (Ay <= 0 && hump == 1)
    {
        absolute_time_t endTime = get_absolute_time();
        uint time = absolute_time_diff_us(startTime, endTime);

        time = time / 1000000; //convert time captured from micro-seconds to seconds
        hump_calculation(accY, time); // call the function to calculate the height of the hump
        // set boolean value back to 0
        hump = 0;
    }

    return true;
}
